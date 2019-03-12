// Adrestia
#include "adrestia_networking.h"
#include "adrestia_hexy.h"

// Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#ifdef __APPLE__
#include <sys/wait.h>
#define MSG_NOSIGNAL 0
#else
#include <wait.h>
#endif
#include <unistd.h>

// System modules
#include <fstream>
#include <iostream>
#include <list>
#include <string>
using namespace std;

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;


string SERVER_IP("127.0.0.1");


class connection_closed {};
class socket_error {};

string hex_urandom(unsigned int number_of_characters) {
	// Creates random hexadecimal string of requested length
	char proto_output[number_of_characters + 1];

	ifstream urandom("/dev/urandom", ios::in|ios::binary);

	if (!urandom) {
		cerr << "Failed to open urandom!\n";
		throw;
	}

	for (unsigned int i = 0; i < number_of_characters; i = i + 1) {
		char next_number = 0;

		urandom.read((char*)(&next_number), sizeof(char));
		next_number &= 0x0F;  // Ensure we generate only one character at a time.

		if (!urandom) {
			cerr << "Failed to read from urandom!\n";
			throw;
		}

		if (next_number < 10) {  // 0-10
			proto_output[i] = (char)(next_number + 48);
		}
		else {  // A-F
			proto_output[i] = (char)(next_number + 87);
		}
	}

	proto_output[number_of_characters] = '\0';

	string returnVar(proto_output);
	return returnVar;
}

string recv_buffer;

string read_packet (int client_socket) {
	// Reads a string sent from the target.
	// The string should end with a newline, although this newline is not returned by this function.
	const int size = 8192;
	char buffer[size];

	while (recv_buffer.find('\n') == string::npos)
	{
		int bytes_read = recv (client_socket, buffer, sizeof(buffer) - 1, 0);
			// Though extremely unlikely in our setting --- connection from
			// localhost, transmitting a small packet at a time --- this code
			// takes care of fragmentation  (one packet arriving could have
			// just one fragment of the transmitted message)

		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			recv_buffer += buffer;
			if (strchr(buffer, '\n') != nullptr) {
				break;
			}
		}

		else if (bytes_read == 0)
		{
			close (client_socket);
			throw connection_closed();
		}

		else
		{
			cerr << "Error " << errno << " (" << strerror(errno) << ")" << endl;
			throw socket_error();
		}
	}

	size_t newline_index = recv_buffer.find('\n');
	string message = recv_buffer.substr(0, newline_index);
	recv_buffer = recv_buffer.substr(newline_index + 1);
	return message;
}

list<json> packet_cache;

json read_packet (int client_socket, string handler_key) {
	// Check the packet cache first
	for (auto it = packet_cache.begin(); it != packet_cache.end(); it++) {
		if ((*it)[adrestia_networking::HANDLER_KEY] == handler_key) {
			json packet = *it;
			packet_cache.erase(it);
			return packet;
		}
	}
	// If that doesn't give you a packet, keep reading packets until we get one with the right handler key
	while (true) {
		string packet_str = read_packet(client_socket);
		json packet = json::parse(packet_str);
		if (packet[adrestia_networking::HANDLER_KEY] == handler_key) {
			return packet;
		}
		packet_cache.push_back(packet);
	}
}


int socket_to_target(const char* IP, int port) {
	// Creates and connects socket to target IP/port
	struct sockaddr_in target_address;

	target_address.sin_family = AF_INET;
	target_address.sin_addr.s_addr = inet_addr(IP);
	target_address.sin_port = htons(port);

	int my_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(my_socket, (struct sockaddr*)&target_address, sizeof(target_address)) == -1) {
		cout << "Error establishing connection: |" << strerror(errno) << "|.\n";
		return -1;
	}

	return my_socket;
}

int main(int argc, char* argv[]) {	
	cout << "Starting sequence." << endl;

	string my_uuid;
	string my_user_name;
	string my_tag;
	string version = version_to_string(adrestia_networking::CLIENT_VERSION);
	string password("test_password");
	string desired_user_name1("test_user");
	string desired_user_name2("test_user_again");

	vector<string> selected_books;
	selected_books.push_back("refinement");
	selected_books.push_back("contrition");
	selected_books.push_back("conjuration");

	vector<string> selected_books_bad1;
	selected_books_bad1.push_back("refinement");
	selected_books_bad1.push_back("contrition");
	selected_books_bad1.push_back("conjuration");
	selected_books_bad1.push_back("bloodlust");

	vector<string> selected_books_bad2;
	selected_books_bad2.push_back("floop");
	selected_books_bad2.push_back("flop");
	selected_books_bad2.push_back("fleep");

	json outbound_json;
	json response_json;
	string outbound_message;

	const char* server_port_env = getenv("SERVER_PORT");
	int port = adrestia_networking::DEFAULT_SERVER_PORT;
	if (server_port_env) {
		port = atoi(server_port_env);
	}

	// Establish connection (socket)
	cout << "Establishing connection (socket)." << endl;
	int my_socket_1 = socket_to_target(SERVER_IP.c_str(), port);
	if (my_socket_1 == -1) {
		cerr << "Failed to establish connection (socket)." << endl;
		return 0;
	}
	else {
		cout << "Successfully connected on socket |" << my_socket_1 << "|." << endl;
	}

	// Establish connection (endpoint)
	cout << "Establishing connection (endpoint)." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_establish_connection_call(outbound_json, version);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "establish_connection");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to establish connection (endpoint)." << endl;
		cerr << "establish_connection says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "establish_connection says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}
	GameRules rules;
	rules = response_json["game_rules"];

	// Create account
	cout << "Registering new account." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_register_new_account_call(outbound_json, password, true, "test1", "test");

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "register_new_account");
	if (response_json[adrestia_networking::CODE_KEY] != 201) {
		cerr << "Failed to register new account." << endl;
		cerr << "register_new_account says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "register_new_account says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		cout << "    uuid: |" << response_json["uuid"] << "|" << endl;
		cout << "    user_name: |" << response_json["user_name"] << "|" << endl;
		cout << "    tag: |" << response_json["tag"] << "|" << endl;

		my_uuid = response_json["uuid"];
		my_user_name = response_json["user_name"];
		my_tag = response_json["tag"];
	}

	// Change user name
	cout << "Changing user name." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_change_user_name_call(outbound_json, desired_user_name1);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "change_user_name");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to change user name." << endl;
		cerr << "change_user_name says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "change_user_name says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		cout << "    tag: |" << response_json["tag"] << "|" << endl;

		my_user_name = desired_user_name1;
		my_tag = response_json["tag"];
	}

	// Terminate connection
	cout << "Closing connection." << endl;
	close(my_socket_1);

	// Establish new connection
	cout << "Establishing new connection (socket)..." << endl;
	my_socket_1 = socket_to_target(SERVER_IP.c_str(), port);
	if (my_socket_1 == -1) {
		cerr << "Failed to establish connection (socket)." << endl;
		return 0;
	}
	else {
		cout << "Successfully connected on socket |" << my_socket_1 << "|." << endl;
	}

	// Establish connection (endpoint)
	cout << "Establishing connection (endpoint)." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_establish_connection_call(outbound_json, version);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "establish_connection");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to establish connection (endpoint)." << endl;
		cerr << "establish_connection says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "establish_connection says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Authenticate
	cout << "Attempting to authenticate with |" << my_uuid << ":" << password << "|..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_authenticate_call(outbound_json, my_uuid, password);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json  = read_packet(my_socket_1, "authenticate");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to authenticate." << endl;
		cerr << "authenticate says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "authenticate says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Matchmake (waiting)
	cout << "Attempting to matchmake (waiting)..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_matchmake_me_call(outbound_json, rules, selected_books);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "matchmake_me");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to enter the matchmaking waiting list." << endl;
		cerr << "matchmake_me says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "matchmake_me says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// We must create a new account in order to be matchmade with the one we just entered.
	// Establish connection (socket)
	cout << "Establishing new connection (socket)." << endl;
	int my_socket_2 = socket_to_target(SERVER_IP.c_str(), port);
	if (my_socket_2 == -1) {
		cerr << "Failed to establish connection (socket)." << endl;
		return 0;
	}
	else {
		cout << "Successfully connected on socket |" << my_socket_2 << "|." << endl;
	}

	// Establish connection (endpoint)
	cout << "Establishing connection (endpoint)." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_establish_connection_call(outbound_json, version);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_2, "establish_connection");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to establish connection (endpoint)." << endl;
		cerr << "establish_connection says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "establish_connection says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Create account
	cout << "Registering new account." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_register_new_account_call(outbound_json, password, true, "test2", "test");

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_2, "register_new_account");
	if (response_json[adrestia_networking::CODE_KEY] != 201) {
		cerr << "Failed to register new account." << endl;
		cerr << "register_new_account says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "register_new_account says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		cout << "    uuid: |" << response_json["uuid"] << "|" << endl;
		cout << "    user_name: |" << response_json["user_name"] << "|" << endl;
		cout << "    tag: |" << response_json["tag"] << "|" << endl;

		my_uuid = response_json["uuid"];
		my_user_name = response_json["user_name"];
		my_tag = response_json["tag"];
	}

	// Matchmake (bad number of selected books)
	cout << "Attempting to matchmake (bad number of selected books)..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_matchmake_me_call(outbound_json, rules, selected_books_bad1);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_2, "matchmake_me");
	if (response_json[adrestia_networking::CODE_KEY] != 400) {
		cerr << "Matchmaking incorrectly succeeded." << endl;
		cerr << "matchmake_me says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "matchmake_me correctly rejected attempt. It says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Matchmake (bad selected books)
	cout << "Attempting to matchmake (bad selected books)..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_matchmake_me_call(outbound_json, rules, selected_books_bad2);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_2, "matchmake_me");
	if (response_json[adrestia_networking::CODE_KEY] != 400) {
		cerr << "Matchmaking incorrectly succeeded." << endl;
		cerr << "matchmake_me says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "matchmake_me correctly rejected attempt. It says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Matchmake (new game)
	cout << "Attempting to matchmake (new game)..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_matchmake_me_call(outbound_json, rules, selected_books);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_2, "matchmake_me");
	if (response_json[adrestia_networking::CODE_KEY] != 201) {
		cerr << "Failed to be matchmade." << endl;
		cerr << "matchmake_me says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "matchmake_me says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Socket 1 should now receive a notification...
	// TODO: Why does it take so long before the client sees this message? The
	// server is sending it almost immediately.
	response_json = read_packet(my_socket_1, "push_active_games");
	string game_uid;
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to receive matchmaking notification on socket 1." << endl;
		cerr << "Received instead:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "push_active_games says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		game_uid = response_json["updates"][0]["game_uid"];
		cout << "Game UID is |" << game_uid << "|" << endl;
	}

	// Socket 2 should also receive a notificatibon...
	cout << "Waiting for other player to get a notification..." << endl;
	response_json = read_packet(my_socket_2, "push_active_games");
	cout << "Other player got a notification." << endl;

	// Try submitting an illegal move
	cout << "Attempting to submit an illegal move" << endl;
	outbound_json.clear();
	adrestia_networking::create_submit_move_call(outbound_json, game_uid, {"conjuration_attack_1", "conjuration_tech"});
	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "submit_move");
	if (response_json[adrestia_networking::CODE_KEY] != 400) {
		cerr << "Server accepted an illegal move." << endl;
	} else {
		cout << "Server rejected an illegal move." << endl;
	}

	// Submit a legal move
	cout << "Submitting a legal move" << endl;
	outbound_json.clear();
	adrestia_networking::create_submit_move_call(outbound_json, game_uid, {"conjuration_tech", "conjuration_attack_1"});
	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "submit_move");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Server rejected a legal move." << endl;
	} else {
		cout << "Server accepted legal move." << endl;
	}

	// Resubmit a legal move
	cout << "Attempting to resubmit a legal move" << endl;
	outbound_json.clear();
	adrestia_networking::create_submit_move_call(outbound_json, game_uid, {"conjuration_tech", "conjuration_attack_1"});
	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "submit_move");
	if (response_json[adrestia_networking::CODE_KEY] != 400) {
		cerr << "Server accepted a duplicate move." << endl;
	} else {
		cout << "Server rejected a depulicate move." << endl;
	}

	// Submit a move for the other player
	cout << "Submitting a legal move for the other player" << endl;
	outbound_json.clear();
	adrestia_networking::create_submit_move_call(outbound_json, game_uid, {"refinement_tech", "refinement_mana"});
	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_2, "submit_move");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Server rejected a legal move for player 2." << endl;
	} else {
		cout << "Server accepted legal move for player 2." << endl;
	}

	// Both players should get a notification now.
	cout << "Waiting for both players to get notifications with updated game views..." << endl;
	response_json = read_packet(my_socket_1, "push_active_games");
	response_json = read_packet(my_socket_2, "push_active_games");
	// TODO: charles: Check these packets to make sure they're sane
	cout << "Both players got notifications." << endl;

	// Player 1 aborts the game
	cout << "Attempting to abort game via player 1..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_abort_game_call(outbound_json, game_uid);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_json = read_packet(my_socket_1, "abort_game");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to abort game." << endl;
		cerr << "abort_game says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_1);
		return 0;
	}
	else {
		cout << "abort_game says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Player 2 should be notified about the game being aborted.
	cout << "Waiting for player 2 to receive game abort push..." << endl;
	response_json = read_packet(my_socket_2, "push_active_games");
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to receive abort notification on socket 2." << endl;
		cerr << "Received instead:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket_2);
		return 0;
	}
	else {
		cout << "push_active_games says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		game_uid = response_json["updates"][0]["game_uid"];
		cout << "Aborted game UID is |" << game_uid << "|" << endl;
	}

	// Done.
	cout << "Done!" << endl;

	close(my_socket_1);
	close(my_socket_2);

	return 0;
}

// Adrestia
#include "adrestia_networking.h"
#include "adrestia_hexy.h"

// Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <wait.h>
#include <unistd.h>

// System modules
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

// JSON
#include "../units_cpp/json.h"
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


string read_packet (int client_socket) {
	// Reads a string sent from the target.
	// The string should end with a newline, although this newline is not returned by this function.
	string msg;

	const int size = 8192;
	char buffer[size];

	while (true)
	{
		int bytes_read = recv (client_socket, buffer, sizeof(buffer) - 2, 0);
			// Though extremely unlikely in our setting --- connection from
			// localhost, transmitting a small packet at a time --- this code
			// takes care of fragmentation  (one packet arriving could have
			// just one fragment of the transmitted message)

		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			buffer[bytes_read + 1] = '\0';

			const char * packet = buffer;
			while (*packet != '\0')
			{
				msg += packet;
				packet += strlen(packet) + 1;

				if (msg.length() > 1 && msg[msg.length() - 1] == '\n')
				{
					return msg.substr(0, msg.length()-1);
				}
			}
		}

		else if (bytes_read == 0)
		{
			close (client_socket);
			throw connection_closed();
		}

		else
		{
			cerr << "Error " << errno << "(" << strerror(errno) << ")" << endl;
			throw socket_error();
		}
	}

	throw connection_closed();
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
	string password("test_password");
	string desired_user_name1("test_user");
	string desired_user_name2("test_user_again");

	vector<string> selected_books;
	selected_books.push_back("refinement");
	selected_books.push_back("contrition");
	selected_books.push_back("conjuration");

	json outbound_json;
	json response_json;
	string outbound_message;
	string response_message;

	// Establish connection (socket)
	cout << "Establishing connection (socket)." << endl;
	int my_socket_1 = socket_to_target(SERVER_IP.c_str(), adrestia_networking::DEFAULT_SERVER_PORT);
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

	adrestia_networking::create_establish_connection_call(outbound_json);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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

	// Create account
	cout << "Registering new account." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_register_new_account_call(outbound_json, password);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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
	my_socket_1 = socket_to_target(SERVER_IP.c_str(), adrestia_networking::DEFAULT_SERVER_PORT);
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

	adrestia_networking::create_establish_connection_call(outbound_json);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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

	adrestia_networking::create_matchmake_me_call(outbound_json, selected_books);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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
	int my_socket_2 = socket_to_target(SERVER_IP.c_str(), adrestia_networking::DEFAULT_SERVER_PORT);
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

	adrestia_networking::create_establish_connection_call(outbound_json);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_2);
	response_json = json::parse(response_message);
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

	adrestia_networking::create_register_new_account_call(outbound_json, password);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_2);
	response_json = json::parse(response_message);
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

	// Matchmake (new game)
	cout << "Attempting to matchmake (new game)..." << endl;
	outbound_json.clear();
	response_json.clear();

	adrestia_networking::create_matchmake_me_call(outbound_json, selected_books);

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket_2, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket_2);
	response_json = json::parse(response_message);
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
	response_message = read_packet(my_socket_1);
	response_json = json::parse(response_message);
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
	}

	// Done.
	cout << "Done!" << endl;

	close(my_socket_1);
	close(my_socket_2);

	return 0;
}

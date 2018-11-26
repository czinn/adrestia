#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <wait.h>
#include <unistd.h>

#include "../units_cpp/json.h"
using json = nlohmann::json;

string SERVER_IP("127.0.0.1");
int SERVER_PORT = 18677;

string HANDLER_KEY_NAME("api_handler_name");


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

	for (int i = 0; i < number_of_characters; i = i + 1) {
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


string read_packet (int client_socket)
{
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


// ACTUAL API STARTS HERE
json register_new_account(const string& password) {
	/* Returns json containing ["id", "user_name", "tag"]. */

	cout << "register_new_account outbound with:" << endl;
	cout << "    password: |" << password << "|" << endl;

	int my_socket = socket_to_target(SERVER_IP.c_str(), SERVER_PORT);

	if (my_socket == -1) {
		cerr << "register_new_account failed to connect." << endl;
		throw string("register_new_account failed to connect.");
	}

	json json_message;
	json_message[HANDLER_KEY_NAME] = "register_new_account";
	json_message["password"] = password;

	string server_send_string = json_message.dump();
	server_send_string += '\n';
	send(my_socket, server_send_string.c_str(), server_send_string.length(), MSG_NOSIGNAL);
	string server_response = read_packet(my_socket);
	json server_response_json = json::parse(server_response);

	cout << "Server reports that it has made for us the following:" << endl;
	cout << "    uuid: |" << server_response_json["uuid"] << "|" << endl;
	cout << "    user_name: |" << server_response_json["user_name"] << "|" << endl;
	cout << "    tag: |" << server_response_json["tag"] << "|" << endl;

	return server_response_json;
}


bool verify_existing_account(const string& uuid, const string& password) {
	cout << "verify_existing_account outbound with:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    password: |" << password << "|" << endl;

	int my_socket = socket_to_target(SERVER_IP.c_str(), SERVER_PORT);
	if (my_socket == -1) {
		cerr << "verify_existing_account failed to connect." << endl;
		throw string("verify_existing_account failed to connect.");
	}

	json json_message;
	json_message[HANDLER_KEY_NAME] = "verify_account";
	json_message["uuid"] = uuid;
	json_message["password"] = password;
	string server_send_string = json_message.dump();
	server_send_string += '\n';
	send(my_socket, server_send_string.c_str(), server_send_string.length(), MSG_NOSIGNAL);

	string server_response = read_packet(my_socket);
	json server_response_json = json::parse(server_response);

	if (server_response_json["api_code"] == 200) {
		cout << "verify_existing_account confirms credentials are valid." << endl;
		return true;
	}
	else if (server_response_json["api_code"] == 401) {
		cout << "verify_existing_account confirms credentials are invalid." << endl;
		return false;
	}

	cout << "verify_existing_account returned unexpected result:" << endl;
	cout << "    code: |" << server_response_json["api_code"] << "|" << endl;
	cout << "    message: |" << server_response_json["api_message"] << "|" << endl;
	return false;
}


json change_user_name(const string& uuid, const string& password, const string& new_user_name) {
	/* Returns the server's response, which will contain key 'tag' if successful. */

	cout << "change_user_name outbound with:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    password: |" << password << "|" << endl;
	cout << "    new_user_name: |" << new_user_name << "|" << endl;

	int my_socket = socket_to_target(SERVER_IP.c_str(), SERVER_PORT);
	if (my_socket == -1) {
		cerr << "change_user_name failed to connect." << endl;
		throw string("change_user_name failed to connect.");
	}

	json json_message;
	json_message[HANDLER_KEY_NAME] = "change_user_name";
	json_message["uuid"] = uuid;
	json_message["password"] = password;
	json_message["new_user_name"] = new_user_name;
	string server_send_string = json_message.dump();
	server_send_string += '\n';
	send(my_socket, server_send_string.c_str(), server_send_string.length(), MSG_NOSIGNAL);

	string server_response = read_packet(my_socket);
	json server_response_json = json::parse(server_response);

	if (server_response_json["api_code"] == 201) {
		cout << "change_user_name reports name has been successfully changed." << endl;
		cout << "New tag: |" << server_response_json["tag"] << "|" << endl;
		return server_response_json;
	}
	else if (server_response_json["api_code"] == 401) {
		cout << "change_user_name reports that the uuid/password given was invalid." << endl;
		return server_response_json;
	}
	cout << "change_user_name reported:" << endl;
	cout << "    code: |" << server_response_json["code"] << "|" << endl;
	cout << "    message: |" << server_response_json["message"] << "|" << endl;
	return server_response_json;
}

// ACTUAL API ENDS HERE


int main(int argc, char* argv[]) {
	string password("blop");
	string desired_user_name("stheno");

	json new_account_info = register_new_account(password);
	bool account_verification = verify_existing_account(new_account_info["uuid"], password);
	json tag_json = change_user_name(new_account_info["uuid"], password, desired_user_name);
}

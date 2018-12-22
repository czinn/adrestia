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

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


string SERVER_IP("127.0.0.1")


class connection_closed {};
class socket_error {};


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
	cout << "Starting sequence.";

	string my_uuid;
	string my_user_name;
	string my_tag;
	string password("test_password");
	string desired_user_name1("test_user");
	string desired_user_name2("test_user_again");

	json outbound_json;
	json response_json;
	string outbound_message;
	string response_message;

	// Establish connection (socket)
	cout << "Establishing connection (socket).";
	int my_socket = socket_to_target(SERVER_IP.c_str(), adrestia_networking::DEFAULT_SERVER_PORT);
	if (my_socket == -1) {
		cerr << "Failed to establish connection (socket)." << endl;
		return;
	}
	else {
		cout << "Successfully connected on socket |" << my_socket << "|." << endl;
	}

	// Establish connection (endpoint)
	cout << "Establishing connection (endpoint).";
	outbound_json.clear();
	response_json.clear();

	outbound_json[adrestia_networking::HANDLER_KEY] = "establish_connection";

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket);
	response_json = json::parse(response_message);
	if (response_json[adrestia_networking::CODE_KEY] != 200) {
		cerr << "Failed to establish connection (endpoint)." << endl;
		cerr << "establish_connection says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket);
		return;
	}
	else {
		cout "establish_connection says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
	}

	// Create account
	cout << "Registering new account." << endl;
	outbound_json.clear();
	response_json.clear();

	outbound_json[adrestia_networking::HANDLER_KEY] = "register_new_account";
	outbound_json["password"] = password;

	outbound_message = outbound_json.dump() + '\n';
	send(my_socket, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
	response_message = read_packet(my_socket);
	response_json = json::parse(response_message);
	if (response_json[adrestia_networking::CODE_KEY] != 201) {
		cerr << "Failed to register new account." << endl;
		cerr << "register_new_account says:" << endl;
		cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
		cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		close(my_socket);
		return;
	}
	else {
		cout "register_new_account says:" << endl;
		cout << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
		cout << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
		cout << "    uuid: |" << respone_json["uuid"] << "|" << endl;
		cout << "    user_name: |" << respone_json["user_name"] << "|" << endl;
		cout << "    tag: |" << response_json["tag"] << "|" << endl;

		my_uuid = response_json["uuid"];
		my_user_name = response_json["user_name"];
		my_tag = response_json["tag"];
	}

	// Change user name
	cout << "Changing user name." << endl;
	


}
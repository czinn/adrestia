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
#include <unordered_map>
#include <stdexcept>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <wait.h>
#include <unistd.h>
#include <time.h>

#define SERVER_PORT 18677
#define MESSAGE_MAX_BYTES 32768
#define MESSAGE_HANDLER_NAME_LENGTH 32
#define Z_DEFAULT "z_default"



class connection_closed {};
class socket_error {};

string hex_urandom(unsigned int number_of_characters) {  // Creates random hex string of requested length
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

int z_default(int client_socket, string recieved_message) {
	string message = "Server does not recognize your request |" + recieved_message + "|.\n";
	send(client_socket, message.c_str(), message.length(), MSG_NOSIGNAL);
	return 0;
}

int floop(int client_socket, string recieved_message) {
	string message = "You've found the floop function!\n";
	send(client_socket, message.c_str(), message.length(), MSG_NOSIGNAL);
	return 0;
}


// The different handler functions, which all take in the socket and message
//     (which will contain the first 32 bytes, their name!) and return an int.
// In case of no match, z_default is called instead. Note that since z_default
//     is less than 32 characters no one can call it normally.
unordered_map<char*, int (*)(int, string)> handler_map = {
	{Z_DEFAULT: z_default},
	{"000000000000000000000000000floop": floop},
};


string read_packet (int client_socket)
{
	// Reads a string sent from the target.
	// The string should end with a newline, although this newline is not returned by this function.
	string msg;

	const int size = MESSAGE_MAX_BYTES;
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


void process_connection(int server_socket, int client_socket) {
	try {
		cout << "[Server] Got new connection: server_socket: |" << server_socket << "|, client_socket: |" << client_socket << "|." << endl;
		string client_request = read_packet(client_socket);
		if (client_request.length() < MESSAGE_HANDLER_NAME_LENGTH) {
			// No function name.
			handler_map.at(Z_DEFAULT)(client_socket, client_request);
		}
		else {
			// We have an actual function name to analyze.
			requested_function_name = client_request.substr(0, MESSAGE_HANDLER_NAME_LENGTH);
			int (*requested_function)(int, string); 
			bool found_requested_function = true;
			try {
				requested_function = handler_map.at(requested_function_name);
			}
			catch (out_of_range& oor) {
				// Target does not exist.
				found_requested_function = false;
			}

			if (found_requested_function) {
				requested_function(client_socket, client_request);
			}
			else {
				handler_map.at(Z_DEFAULT)(client_socket, client_request);
			}
		}
		cout << "[Server] Closing this connection: server_socket: |" << server_socket << "|, client_socket: |" << client_socket << "|." << endl;
		close(client_socket);
	}
	catch (connection_closed) {
		cerr << "[Server] The client has closed their connection!" << endl;
		return;
	}
	catch (socket_error) {
		cerr << "Socket error" << endl;
	}
}


void listen_for_connections(int port) {
	int server_socket;
	int client_socket;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	socklen_t client_address_sizeof;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);

	if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
	{
		cout << "Could not bind socket to address:port: |" << strerror(errno) << "|" << endl;
		throw socket_error();
	}

	listen(server_socket, 5);

	while (true) {
		client_address_sizeof = sizeof(client_address);
		client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_sizeof);

		pid_t pid = fork();
		if (pid == 0) {
			// We are the child; our responsibility is to process the new connection.
			close(server_socket);  // The parent will keep listening.

			if (fork() == 0) {  // detach us (parent returns immediately)
				usleep(10000);  // Sleep to allow parent to finish
				process_connection(server_socket, client_socket);
			}

			return;
		} 
		else if (pid > 0) {
			// We are the parent; our responsibility is to keep listening.
			int status = 0;
			waitpid(pid, &status, 0);
			close(client_socket);
		}
		else {
			// An error!
			cerr << "ERROR on fork() with code |" << pid << "|." << endl;
			return;
		}
	}
}


int main(int na, char* arg[]) {
	listen_for_connections(SERVER_PORT);
	return 0;
}
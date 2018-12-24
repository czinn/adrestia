/* The actual server. Run this to run the server. */

// Us
#include "adrestia_networking.h"

// Networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <wait.h>
#include <unistd.h>
#include <time.h>

// System headers
#include <map>
#include <string>
using namespace std;

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;

class connection_closed {};
class socket_error {};

// Map from message kinds to functions that handle them. Message kind is stored in field HANDLER_KEY_NAME.
std::map<string, adrestia_networking::request_handler> handler_map;

string read_message_buffer;


string adrestia_networking::read_message (int client_socket) {
	/* @brief Extracts the message currently waiting on the from the client's socket.
	 *        Assumes message is terminated with a \n (if it is not, MESSAGE_MAX_BYTES will be read.
	 */
	// TODO: Timeouts
	char buffer[MESSAGE_MAX_BYTES];

	bool complete = read_message_buffer.find('\n') != string::npos;
	while (!complete) {
		int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 2, 0);
		if (bytes_read > 0) {
			for (const char *c = buffer; c < buffer + bytes_read; c++) {
				read_message_buffer += *c;
				if (*c == '\n') complete = true;
			}
		} else if (bytes_read == 0) {
			close (client_socket);
			throw connection_closed();
		} else {
			cerr << "Error " << errno << "(" << strerror(errno) << ")" << endl;
			throw socket_error();
		}
	}

	size_t nl_pos = read_message_buffer.find('\n');
	string message = read_message_buffer.substr(0, nl_pos);
	read_message_buffer = read_message_buffer.substr(nl_pos + 1);
	return message;
}


void adrestia_networking::babysit_client(int server_socket, int client_socket) {
	/* A single thread lives within this function, babysitting a client's connection.
	 *
	 * The connection proceeds through several phases:
	 *     Phase 0: Connection is not formally 'established'.
	 *              Anything other than an establish_connection request is rejected.
	 *     Phase 1: Connection is established but unauthenticated.
	 *              Anything other than "register_new_account" or "authenticate" are rejected.
	 *     Phase 2: Authenticated connection.
	 *
	 * While the individual handler functions may return their own things, in case of error,
	 *     expect the following keys:
	 *     HANDLER_KEY: generic_error
	 *     CODE_KEY: 400
	 *     MESSAGE_KEY: <A message describing the problem>
	 */

	unsigned int phase = 0;
	string uuid = "";  // The uuid of the client we are babysitting

	try {
		json client_json;
		json resp;
		
		while (true) {
			resp.clear();
			// read message
			string message = read_message(client_socket);

			string requested_function_name;

			bool have_valid_function_to_call = true;
			try {
				client_json = json::parse(message);
				requested_function_name = client_json.at(HANDLER_KEY_NAME);
				adrestia_networking::request_handler requested_function = handler_map.at(requested_function_name);

				if ((phase == 0) && (requested_function_name.compare("establish_connection") != 0)) {
					cout << "[Server] received out-of-order request for function |" << requested_function_name << "|." << endl;

					resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
					resp[adrestia_networking::CODE_KEY] = 400;
					resp[adrestia_networking::MESSAGE_KEY] = "received out-of-order request for function |" + requested_function_name + "|.";
					have_valid_function_to_call = false;				
				}
				else if ((phase == 1) && !((requested_function_name.compare("register_new_account") == 0) ||
					                       (requested_function_name.compare("authenticate") == 0)
					                      )
					    ) {
					cout << "[Server] received out-of-order request for function |" << requested_function_name << "|." << endl;

					resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
					resp[adrestia_networking::CODE_KEY] = 400;
					resp[adrestia_networking::MESSAGE_KEY] = "received out-of-order request for function |" + requested_function_name + "|.";
					have_valid_function_to_call = false;
				}

				// We have a function that we can actually work with...
				cout << "[Server] received valid call for function |" << requested_function_name << "| in phase |" << phase << "|." << endl;

				if (requested_function_name.compare("establish_connection") == 0) {
					requested_function(client_json, resp);
					phase = 1;
				}
				else if (requested_function_name.compare("register_new_account") == 0) {
					requested_function(client_json, resp);

					// This is a type of authentication (and it always succeeds)
					uuid = resp["uuid"]
					phase = 2;
				}
				else if (requested_function_name.compare("authenticate") == 0) {
					int valid_authentication = requested_function(client_json, resp);

					if (valid_authentication) {
						uuid = client_json["uuid"];
						phase = 2;
					}
				}
				else {
					// This is all authenticated functions. uuid will be added to client_json.
					client_json["uuid"] = uuid;
					requested_function(client_json, resp);
				}
			}
			catch (json::parse_error& e) {
				// Not parsable to json
				cout << "[Server] Did not receive json-y message. " << e.what() << endl;
				resp.clear();
				resp[adrestia_networking::HANDLER_KEY] = "generic_error";
				resp[adrestia_networking::CODE_KEY] = 400;
				resp[adrestia_networking::MESSAGE_KEY] = "Could not parse your message as json.";
			}
			catch (json::out_of_range& e) {
				// Does not contain all required fields
				cout << "[Server] Client json did not have an expected key. " << e.what() << endl;
				resp[adrestia_networking::HANDLER_KEY] = "generic_error";
				resp[adretia_networking::CODE_KEY] = 400;
				resp[adretia_networking::MESSAGE_KEY] = "Could not find an expected key: |" + e.what() + "|.";
			}
			catch (out_of_range& oor) {
				// Asked to access non-existent function
				cout << "[Server] Asked to access non-existent function |" << requested_function_name << "|" << endl;
				resp[adrestia_networking::HANDLER_KEY] = "generic_error";
				resp[adrestia_networking::CODE_KEY] = 400;
				resp[adrestia_networking::MESSAGE_KEY] = "Asked to access non-existent endpoint |" + requested_function_name + "|.";
			}

			string response_string = resp.dump();
			response_string += '\n';
			send(client_socket, response_string.c_str(), response_string.length(), MSG_NOSIGNAL);
		}
	}
	catch (connection_closed) {
		cout << "[Server] Client |" << client_socket << "| closed the connection." << endl << endl;
	}
	catch (socket_error) {
		cout << "Socket error." << endl;
	}
}


void adrestia_networking::listen_for_connections(int port) {
	int server_socket;
	int client_socket;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	socklen_t client_address_sizeof;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);

	if (bind(server_socket, (sockaddr*) &server_address, sizeof(server_address)) == -1) {
		cout << "Could not bind socket to address:port: |" << strerror(errno) << "|" << endl;
		throw socket_error();
	}

	listen(server_socket, 5);

	while (true) {
		client_address_sizeof = sizeof(client_address);
		client_socket = accept(server_socket, (sockaddr*) &client_address, &client_address_sizeof);

		pid_t pid = fork();
		if (pid == 0) {
			// We are the child; our responsibility is to process the new connection.
			close(server_socket);  // The parent will keep listening.

			if (fork() == 0) {  // detach us (parent returns immediately)
				usleep(10000);  // Sleep to allow parent to finish
				babysit_client(server_socket, client_socket);
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
	handler_map["floop"] = adrestia_networking::handle_floop;
	handler_map["establish_connection"] = adrestia_networking::establish_connection;
	handler_map["register_new_account"] = adrestia_networking::handle_register_new_account;
	handler_map["authenticate"] = adrestia_networking::handle_authenticate;

	const char* server_port_env = getenv("SERVER_PORT");
	int port = adrestia_newtorking::DEFAULT_SERVER_PORT;
	if (server_port_env) {
		port = atoi(server_port_env)
	}

	cout << "Listening for connections on port " << port << "." << endl;
	adrestia_networking::listen_for_connections(port);
	return 0;
}
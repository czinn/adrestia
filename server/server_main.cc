/* The actual server. Run this to run the server. */

// Us
#include "adrestia_networking.h"
#include "adrestia_hexy.h"

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
#include <iostream>
#include <map>
#include <string>
using namespace std;

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;

class connection_closed {};
class socket_error {};

// Map from message kinds to functions that handle them. Message kind is stored in field HANDLER_KEY_NAME.
std::map<string, adrestia_networking::request_handler> handler_map;

string read_message_buffer;

string adrestia_networking::read_message (int client_socket, bool& timed_out) {
	/* @brief Extracts the message currently waiting on the from the client's socket.
	 *        Assumes message is terminated with a \n (if it is not, MESSAGE_MAX_BYTES will be read.
	 */
	// TODO: Timeouts
	char buffer[MESSAGE_MAX_BYTES];

	timed_out = false;

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
		} else if (bytes_read == -1) {
			timed_out = true;
			complete = true;
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

	// We keep an id for logging purposes
	const string babysitter_id = adrestia_hexy::hex_urandom(8);
	cout << "Babysitter |" << babysitter_id << "| starting sequence." << endl;

	unsigned int phase = 0;
	string uuid = "";  // The uuid of the client we are babysitting
	map<string, string> games_I_am_aware_of; // game_uid to game_state
	vector<string> active_game_uids_I_am_aware_of;

	try {
		json client_json;
		json resp;
		
		while (true) {
			// Pushers
			if (phase == 2) {
				cout << "[" << babysitter_id << "] Processing pushers..." << endl;

				json message_json;
				message_json.clear();
				adrestia_networking::push_active_games(babysitter_id,
					                                   message_json,
					                                   uuid,
					                                   games_I_am_aware_of,
					                                   active_game_uids_I_am_aware_of
					                                  );

				if (message_json.at(adrestia_networking::CODE_KEY) == 200) {
					// We should push the json to the client.
					cout << "[" << babysitter_id << "] Pushing new/changed game notification to client." << endl;
					string message_json_string = message_json.dump();
					message_json_string += '\n';
					send(client_socket, message_json_string.c_str(), message_json_string.length(), MSG_NOSIGNAL);
				} else {
					cout << "[" << babysitter_id << "] Not pushing new/changed game notification to client due to code |" << message_json.at(adrestia_networking::CODE_KEY) << "|" << endl;
				}
			}

			// read message
			resp.clear();
			bool timed_out = false;
			string message = read_message(client_socket, timed_out);

			if (timed_out) {
				continue;
			}

			string requested_function_name;

			bool have_valid_function_to_call = true;
			try {
				client_json = json::parse(message);
				requested_function_name = client_json.at(adrestia_networking::HANDLER_KEY);
				adrestia_networking::request_handler requested_function = handler_map.at(requested_function_name);

				if ((phase == 0) && (requested_function_name.compare("establish_connection") != 0)) {
					cout << "[" << babysitter_id << "] received out-of-order request for function |"
					     << requested_function_name
					     << "|."
					     << endl;

					resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
					resp[adrestia_networking::CODE_KEY] = 400;
					resp[adrestia_networking::MESSAGE_KEY] = "received out-of-order request for function |" +
					                                         requested_function_name +
					                                         "|.";
					have_valid_function_to_call = false;
				}
				else if ((phase == 1) && !((requested_function_name.compare("register_new_account") == 0) ||
					                       (requested_function_name.compare("authenticate") == 0)
					                      )
					    ) {
					cout << "[" << babysitter_id << "] received out-of-order request for function |"
					     << requested_function_name
					     << "| in phase |"
					     << phase
					     << "|"
					     << endl;

					resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
					resp[adrestia_networking::CODE_KEY] = 400;
					resp[adrestia_networking::MESSAGE_KEY] = "received out-of-order request for function |" + 
					                                         requested_function_name +
					                                         "|";
					have_valid_function_to_call = false;
				}

				if (have_valid_function_to_call) {
					// We have a function that we can actually work with...
					cout << "[" << babysitter_id << "] received valid call for function |"
					     << requested_function_name
					     << "| in phase |"
					     << phase
					     << "|."
					     << endl;

					if (requested_function_name.compare("establish_connection") == 0) {
						requested_function(babysitter_id, client_json, resp);
						cout << "[" << babysitter_id << "] moving to phase 1." << endl;;
						phase = 1;
					}
					else if (requested_function_name.compare("register_new_account") == 0) {
						requested_function(babysitter_id, client_json, resp);

						// This is a type of authentication (and it always succeeds)
						uuid = resp["uuid"];
						cout << "[" << babysitter_id << "] moving to phase 2 via register_new_account." << endl;
						phase = 2;
					}
					else if (requested_function_name.compare("authenticate") == 0) {
						int valid_authentication = requested_function(babysitter_id, client_json, resp);

						if (valid_authentication == 0) {
							uuid = client_json["uuid"];
							cout << "[" << babysitter_id << "] moving to phase 2 via successful authenticate." << endl;
							phase = 2;
						}
					}
					else {
						// This is all authenticated functions. uuid will be added to client_json.
						client_json["uuid"] = uuid;
						requested_function(babysitter_id, client_json, resp);
					}
				}
			}
			catch (json::parse_error& e) {
				// Not parsable to json
				cout << "[" << babysitter_id << "] Did not receive json-y message. " << e.what() << endl;
				resp.clear();
				resp[adrestia_networking::HANDLER_KEY] = "generic_error";
				resp[adrestia_networking::CODE_KEY] = 400;
				resp[adrestia_networking::MESSAGE_KEY] = "Could not parse your message as json.";
			}
			catch (json::out_of_range& e) {
				// Does not contain all required fields
				cout << "[" << babysitter_id << "] Client json did not have an expected key. " << e.what() << endl;
				resp[adrestia_networking::HANDLER_KEY] = "generic_error";
				resp[adrestia_networking::CODE_KEY] = 400;
				resp[adrestia_networking::MESSAGE_KEY] = "Could not find an expected key: |" + string(e.what()) + "|.";
			}
			catch (out_of_range& oor) {
				// Asked to access non-existent function
				cout << "[" << babysitter_id << "] Asked to access non-existent function |" << requested_function_name << "|" << endl;
				resp[adrestia_networking::HANDLER_KEY] = "generic_error";
				resp[adrestia_networking::CODE_KEY] = 400;
				resp[adrestia_networking::MESSAGE_KEY] = "Asked to access non-existent endpoint |" +
				                                         requested_function_name +
				                                         "|.";
			}

			string response_string = resp.dump();
			response_string += '\n';
			send(client_socket, response_string.c_str(), response_string.length(), MSG_NOSIGNAL);
		}
	}
	catch (connection_closed) {
		cout << "[" << babysitter_id << "] Client closed the connection." << endl << endl;
	}
	catch (socket_error) {
		cout << "[" << babysitter_id << "] terminating due to socket error." << endl;
	}
}


void adrestia_networking::listen_for_connections(int port) {
	int server_socket;
	int client_socket;

	sockaddr_in server_address;
	sockaddr_in client_address;
	socklen_t client_address_sizeof;

	timeval recv_timeout;
	recv_timeout.tv_sec = adrestia_networking::WAIT_FOR_COMMANDS_SECONDS;
	recv_timeout.tv_usec = 0;
	timeval send_timeout;
	send_timeout.tv_sec = adrestia_networking::TIMEOUT_SEND_SECONDS;
	send_timeout.tv_usec = 0;

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

		// Configure our timeouts and keepalives
		// TODO: KEEPALIVE
		if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(timeval)) < 0) {
			cerr << "ERROR on setsockopt for client socket (recieve timeout)!" << endl;
			return;
		}
		if (setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(timeval)) < 0) {
			cerr << "ERROR on setsockopt for client socket (send timeout)!" << endl;
			return;
		}

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

	handler_map["establish_connection"] = adrestia_networking::handle_establish_connection;

	handler_map["register_new_account"] = adrestia_networking::handle_register_new_account;
	handler_map["authenticate"] = adrestia_networking::handle_authenticate;

	handler_map["change_user_name"] = adrestia_networking::handle_change_user_name;
	handler_map["matchmake_me"] = adrestia_networking::handle_matchmake_me;


	const char* server_port_env = getenv("SERVER_PORT");
	int port = adrestia_networking::DEFAULT_SERVER_PORT;
	if (server_port_env) {
		port = atoi(server_port_env);
	}

	cout << "Listening for connections on port " << port << "." << endl;
	adrestia_networking::listen_for_connections(port);
	return 0;
}

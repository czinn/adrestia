/* The actual server. Run this to run the server. */

// Us
#include "adrestia_networking.h"
#include "adrestia_hexy.h"
#include "adrestia_database.h"
#include "babysitter.h"
#include "logger.h"

// Networking
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
#include <time.h>

// System headers
#include <iostream>
#include <map>
#include <string>
#include <thread>
using namespace std;

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;

void babysit(int client_socket) {
	logger.prefix = adrestia_hexy::hex_urandom(8);
	Babysitter(client_socket).main();
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

	int enable = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		cout << "Failed to set setsockopt(SO_REUSEADDR)" << endl;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);

	if (::bind(server_socket, (sockaddr*) &server_address, sizeof(server_address)) == -1) {
		cout << "Could not bind socket to address:port: |" << strerror(errno) << "|" << endl;
		throw socket_error();
	}

	listen(server_socket, 5);

	std::vector<std::thread> babysitters;

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

		babysitters.emplace_back(babysit, client_socket);
	}
}

int main(int na, char* arg[]) {
	handler_map["floop"] = adrestia_networking::handle_floop;

	handler_map["establish_connection"] = adrestia_networking::handle_establish_connection;

	handler_map["register_new_account"] = adrestia_networking::handle_register_new_account;
	handler_map["authenticate"] = adrestia_networking::handle_authenticate;

	handler_map["abort_game"] = adrestia_networking::handle_abort_game;
	handler_map["change_user_name"] = adrestia_networking::handle_change_user_name;
	handler_map["matchmake_me"] = adrestia_networking::handle_matchmake_me;
	handler_map["submit_move"] = adrestia_networking::handle_submit_move;
	handler_map["get_stats"] = adrestia_networking::handle_get_stats;
	handler_map["deactivate_account"] = adrestia_networking::handle_deactivate_account;

	handler_map["get_user_profile"] = adrestia_networking::handle_get_user_profile;
	handler_map["follow_user"] = adrestia_networking::handle_follow_user;
	handler_map["unfollow_user"] = adrestia_networking::handle_unfollow_user;
	handler_map["get_friends"] = adrestia_networking::handle_get_friends;
	handler_map["send_challenge"] = adrestia_networking::handle_send_challenge;

	const char* server_port_env = getenv("SERVER_PORT");
	int port = adrestia_networking::DEFAULT_SERVER_PORT;
	if (server_port_env) {
		port = atoi(server_port_env);
	}

	// clear out ephemeral tables where left over bits may mess things up
	logger.prefix = "main";
	{
		adrestia_database::Db db;
		db.query("DELETE FROM adrestia_match_waiters")();
		db.query("DELETE FROM challenges")();
		db.commit();
	}

	cout << "Listening for connections on port " << port << "." << endl;
	adrestia_networking::listen_for_connections(port);
	return 0;
}

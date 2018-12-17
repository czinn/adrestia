#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <wait.h>
#include <unistd.h>
#include <time.h>

#include <openssl/evp.h>
#include <pqxx/pqxx>

#include "protocol.h"
#include "config.h"

#include "../units_cpp/json.h"
using json = nlohmann::json;

string HANDLER_KEY_NAME("api_handler_name");
// All responses will be json containing key 'api_code' and 'api_message', possibly other keys.

class connection_closed {};
class socket_error {};

string hex_urandom(size_t number_of_characters) {  // Creates random hex string of requested length
	char proto_output[number_of_characters + 1];

	ifstream urandom("/dev/urandom", ios::in|ios::binary);

	if (!urandom) {
		cerr << "Failed to open urandom!\n";
		throw;
	}

	for (size_t i = 0; i < number_of_characters; i = i + 1) {
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

// CRYPTO STUFF BEGINS -- put this in another file!
void digest_message(
	const char* message,
	size_t message_length,
	unsigned char** digest_returncarrier,
	unsigned int* digest_length_returncarrier
) {
	EVP_MD_CTX* mdctx;

	if ((mdctx = EVP_MD_CTX_new()) == nullptr) {
		cerr << "Indigestion type 1!" << endl;
		throw string("Indigestion type 1.");
	}
	if (1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), nullptr)) {
		cerr << "Indigestion type 2!" << endl;
		throw string("Indigestion type 2.");
	}
	if (1 != EVP_DigestUpdate(mdctx, message, message_length)) {
		cerr << "Indigestion type 3!" << endl;
		throw string("Indigestion type 3.");
	}
	if ((*digest_returncarrier = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha512()))) == nullptr) {
		cerr << "Indigestion type 4!" << endl;
		throw string("Indigestion type 4.");
	}
	if (1 != EVP_DigestFinal_ex(mdctx, *digest_returncarrier, digest_length_returncarrier)) {
		cerr << "Indigestion type 5!" << endl;
		throw string("Indigestion type 5.");
	}

	EVP_MD_CTX_free(mdctx);
}
// CRYPTO STUFF ENDS

// DATABASE STUFF BEGINS -- Put this in another file!
void print_hexy(const char* not_hexy, int length) {
	stringstream ss;
	ss << "HEXY: |";
	for (int i = 0; i < length; i += 1) {
		ss << ":";
		ss << hex << (int)not_hexy[i];
		ss << ";";
	}
	ss << "|";
	cout << ss.str();
}


json adjust_user_name_in_database(
	pqxx::connection* psql_connection,
	const string& uuid,
	const string& user_name)
{
	/* Returns json with [tag] */
	const string update_user_name_command = ""
	"UPDATE adrestia_accounts"
	"    SET user_name = $1, tag = $2"
	"    WHERE uuid = $3"
	";";

	cout << "adjust_user_name_in_database called with args:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    user_name: |" << user_name << "|" << endl;

	json new_account_info;

	bool successfully_updated = false;
	psql_connection[0].prepare("update_user_name_command",
	                           update_user_name_command);
	for (int i = 0; i < 1000; i += 1) {
		string tag = hex_urandom(TAG_LENGTH);

		pqxx::work insertion_transaction(psql_connection[0]);
		try {
			pqxx::result statement_result = insertion_transaction.prepared("update_user_name_command")
			                                                              (user_name)(tag)(uuid)
			                                                              .exec();
			insertion_transaction.commit();

			cout << "Successfully adjustment of user_name in database." << endl;
			successfully_updated = true;
			new_account_info["tag"] = tag;
			break;
		}
		catch (pqxx::integrity_constraint_violation) {
			insertion_transaction.abort();
			continue;
		}
	}

	if (!successfully_updated) {
		cout << "Failed to update the user_name!";
		throw string("Failed to update user name of uuid |" + uuid + "| to user_name |" + user_name + "|!");
	}

	return new_account_info;
}


json register_new_account_in_database(
	pqxx::connection* psql_connection,
	const string& password)
{
	/* Returns json with [id, user_name, tag] */
	const string insert_new_account_into_database_command = ""
	"INSERT INTO adrestia_accounts (uuid, user_name, tag, hash_of_salt_and_password, salt)"
	"    VALUES ($1, $2, $3, $4, $5)"
	";";

	const string default_user_name = "Initiate";

	cout << "register_new_account_in_database called with args:" << endl;
	cout << "    password: |" << password << "|" << endl;

	// Get hash of salt and password
	string salt = hex_urandom(SALT_LENGTH);
	string salt_and_password = salt + password;
	const char* salt_and_password_c_str = salt_and_password.c_str();
	unsigned char* hash_of_salt_and_password = new unsigned char[EVP_MAX_MD_SIZE];
	unsigned int hash_of_salt_and_password_length;
	digest_message(salt_and_password_c_str, salt_and_password.length(),
	               &hash_of_salt_and_password, &hash_of_salt_and_password_length
	              );
	string good_string = std::string(reinterpret_cast<const char *>(hash_of_salt_and_password),
	                                 (size_t)hash_of_salt_and_password_length
	                                );
	pqxx::binarystring bstring(good_string);

	json new_account;
	bool actually_created_account = false;

	// Keep making up ids/tags until we get a successful insertion.
	psql_connection[0].prepare("insert_new_account_into_database_command",
	                           insert_new_account_into_database_command
	                          );
	for (int i = 0; i < 1000; i += 1) {
		string uuid = hex_urandom(UUID_LENGTH);
		string tag = hex_urandom(TAG_LENGTH);

		pqxx::work insertion_transaction(psql_connection[0]);
		try {
			pqxx::result statement_result = insertion_transaction.prepared("insert_new_account_into_database_command")
			                                                              (uuid)(default_user_name)(tag)(bstring)(salt)
			                                                              .exec();
			insertion_transaction.commit();

			cout << "Successfully finished insertion of new account into database." << endl;
			actually_created_account = true;
			new_account["uuid"] = uuid;
			new_account["tag"] = tag;
			new_account["user_name"] = default_user_name;
			break;
		}
		catch (pqxx::integrity_constraint_violation) {
			insertion_transaction.abort();
			continue;
		}
	}

	if (!actually_created_account) {
		throw string("Failed to generate non-conflicing uuid/tag pair.");
	}

	delete hash_of_salt_and_password;

	return new_account;
}


bool verify_existing_account_in_database(
	pqxx::connection* psql_connection,
	const string& uuid,
	const string& password)
{
	const string select_password_from_database_command = ""
	"SELECT hash_of_salt_and_password, salt"
	"    FROM adrestia_accounts"
	"    WHERE uuid = $1"
	";";

	cout << "verify_existing_account_in_database called with args:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    password: |" << password << "|" << endl;

	// Find account of given name
	psql_connection[0].prepare("select_password_from_database_command", select_password_from_database_command);
	pqxx::work select_transaction(psql_connection[0]);
	pqxx::result search_result = select_transaction.prepared("select_password_from_database_command")
	                                                        (uuid).exec();
	select_transaction.commit();

	if (search_result.size() == 0) {
		cout << "This uuid not found in database.";
		return false;
	}

	pqxx::binarystring database_hash_of_salt_and_password(search_result[0]["hash_of_salt_and_password"]);
	string database_salt(search_result[0]["salt"].c_str());

	// Get expected hashed password.
	// The hashing of the password only in the case that the account exists is bad for security, but good for speed!
	string salt_and_password = database_salt + password;
	const char* salt_and_password_c_str = salt_and_password.c_str();
	unsigned char* hash_of_salt_and_password = new unsigned char[EVP_MAX_MD_SIZE];
	unsigned int hash_of_salt_and_password_length;
	digest_message(salt_and_password_c_str,
	               salt_and_password.length(),
	               &hash_of_salt_and_password,
	               &hash_of_salt_and_password_length
	              );
	string good_string = std::string(reinterpret_cast<const char *>(hash_of_salt_and_password),
	                                 (size_t)hash_of_salt_and_password_length
	                                );
	pqxx::binarystring expected_hash_of_salt_and_password(good_string);

	if (expected_hash_of_salt_and_password == database_hash_of_salt_and_password) {
		cout << "This uuid and password have been verified." << endl;
		return true;
	}

	cout << "Received an incorrect password for this known uuid." << endl;
	return false;
}


pqxx::connection* establish_psql_connection() {
	const char *db_conn_string = getenv("DB_CONNECTION_STRING");
	if (db_conn_string == nullptr) {
		cerr << "Failed to read DB_CONNECTION_STRING from env." << endl;
		throw string("Failed to read DB_CONNECTION_STRING from env.");
	}
	pqxx::connection* psql_connection = new pqxx::connection(db_conn_string);
	return psql_connection;
}
// DATABASE STUFF ENDS


int handle_register_new_account(const json& client_json, json &resp) {
	cout << "Triggered register_new_account." << endl;
	string password = client_json.at("password");

	cout << "Creating new account with params:" << endl;
	cout << "    password: |" << password << "|" << endl;

	pqxx::connection* psql_connection = establish_psql_connection();
	json new_account = register_new_account_in_database(psql_connection, password);
	delete psql_connection;

	cout << "Created new account with:" << endl;
	cout << "    uuid: |" << new_account["uuid"] << "|" << endl;
	cout << "    user_name: |" << new_account["user_name"] << "|" << endl;
	cout << "    tag: |" << new_account["tag"] << "|" << endl;

	cout << "Returning this data to client..." << endl;
	write_register_new_account_response(resp,
			new_account["uuid"],
			new_account["user_name"],
			new_account["tag"]);

	cout << "register_new_account concluded." << endl;
	return 0;
}


int handle_verify_account(const json& client_json, json &resp) {
	cout << "Triggered verify_account." << endl;
	string uuid = client_json.at("uuid");
	string password = client_json.at("password");

	cout << "Checking authentication for account with:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    password: |" << password << "|" << endl;
	pqxx::connection* psql_connection = establish_psql_connection();
	bool valid = verify_existing_account_in_database(psql_connection, uuid, password);
	delete psql_connection;

	write_verify_account_response(resp, valid);

	cout << "verify_account concluded." << endl;
	return 0;
}


int handle_change_user_name(const json& client_json, json &resp) {
	cout << "Triggered change_user_name." << endl;
	string uuid = client_json.at("uuid");
	string password = client_json.at("password");
	string new_user_name = client_json.at("new_user_name");

	// Authenticate this change.
	pqxx::connection* psql_connection = establish_psql_connection();
	bool authorized_change = verify_existing_account_in_database(psql_connection, uuid, password);

	if (authorized_change) {
		cout << "Change request authenticated; commencing." << endl;
		json new_account_info = adjust_user_name_in_database(psql_connection, uuid, new_user_name);

		cout << "New account info is:" << endl;
		cout << "    uuid: |" << uuid << "|" << endl;
		cout << "    user_name: |" << new_user_name << "|" << endl;
		cout << "    tag: |" << new_account_info["tag"] << "|" << endl;

		write_change_user_name_response(resp, new_account_info["tag"]);
	} else {
		cout << "uuid/password mismatch; cannot perform operation." << endl;
		write_change_user_name_response_unauthorized(resp);
	}

	delete psql_connection;
	return 0;
}


int handle_floop(const json& client_json, json &resp) {
	write_floop_response(resp);
	return 0;
}


// Map from message kinds to functions that handle them. Message kind is stored
// in field HANDLER_KEY_NAME.
typedef std::function<int(const json&, json&)> request_handler;
std::map<string, request_handler> handler_map;

// jim: I believe that each process has its own copy of this, which starts off
// empty.
string read_message_buffer; 

// Returns the next message from the client. Messages are separated by \n.
string read_message (int client_socket) {
	char buffer[MESSAGE_MAX_BYTES];

	bool complete = read_message_buffer.find('\n') != string::npos;
	while (!complete) {
		int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 2, 0);
		cout << "Got some data from socket |" << client_socket << "|" << endl;
		if (bytes_read > 0) {
			for (const char *c = buffer; c < buffer + bytes_read; c++) {
				read_message_buffer += *c;
				if (*c == '\n') complete = true;
			}
			cout << endl;
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

void process_connection(int server_socket, int client_socket) {
	try {
		cout << "[Server] Got new connection: server_socket: |" << server_socket << "|, client_socket: |" << client_socket << "|." << endl;
		while (true) {
			string message = read_message(client_socket);

			json client_json;
			json resp;
			string requested_function_name;

			try {
				client_json = json::parse(message);
				requested_function_name = client_json.at(HANDLER_KEY_NAME);
				request_handler requested_function = handler_map.at(requested_function_name);
				cout << "[Server] Activating function |" << requested_function_name << "|." << endl;
				requested_function(client_json, resp);
			}
			catch (json::parse_error& e) {
				// Not parsable to json
				cout << "[Server] Did not receive json-y message. " << e.what() << endl;
				resp.clear();
				write_error(resp);
			}
			catch (json::out_of_range& e) {
				// Does not contain all required fields
				// TODO: jim: Print just the name of the missing key, not the entire
				// exception explanation.
				cout << "[Server] Request object did not have an expected key. " << e.what() << endl;
				resp.clear();
				write_missing_key_error(resp, e.what());
			}
			catch (out_of_range& oor) {
				// Asked to access non-existent function.
				cout << "[Server] Asked to access non-existent function |" << requested_function_name << "|." << endl;
				resp.clear();
				write_error(resp);
			}

			string resp_str = resp.dump();
			resp_str += '\n';
			send(client_socket, resp_str.c_str(), resp_str.length(), MSG_NOSIGNAL);
		}
		cout
			<< "[Server] Closing this connection: server_socket: |" << server_socket
			<< "|, client_socket: |" << client_socket << "|." << endl << endl;
		close(client_socket);
	}
	catch (connection_closed) {
		cout << "[Server] Client |" << client_socket << "| closed the connection." << endl << endl;
	}
	catch (socket_error) {
		cout << "Socket error" << endl;
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
	handler_map["floop"] = handle_floop;
	handler_map["register_new_account"] = handle_register_new_account;
	handler_map["verify_account"] = handle_verify_account;
	handler_map["change_user_name"] = handle_change_user_name;
	//handler_map["delete_account"] = delete_account;
	
	const char *server_port_env = getenv("SERVER_PORT");
	int port = DEFAULT_SERVER_PORT;
	if (server_port_env) {
		port = atoi(server_port_env);
	}

	cout << "Listening for connections on port " << port << "." << endl;
	listen_for_connections(port);
	return 0;
}

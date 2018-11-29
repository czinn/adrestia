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
#include <fstream>
#include <string>
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

#include <openssl/evp.h>
#include <pqxx/pqxx>

#include "../units_cpp/json.h"
using json = nlohmann::json;

#define SERVER_PORT 18677
#define MESSAGE_MAX_BYTES 32768
#define MESSAGE_HANDLER_NAME_LENGTH 32

string HANDLER_KEY_NAME("api_handler_name");
string Z_DEFAULT_FUNCTION_NAME("z_default");
// All responses will be json containing key 'api_code' and 'api_message', possibly other keys.

#define ENV_FILE_PATH ".env"

#define SALT_LENGTH 16
#define UUID_LENGTH 32
#define TAG_LENGTH 8

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
	                           update_user_name_command
	                          );
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


pqxx::connection* establish_psql_connection(const string& connection_config_string) {
	pqxx::connection* psql_connection = new pqxx::connection(connection_config_string);
	return psql_connection;
}
// DATABASE STUFF ENDS


string get_database_connection_config_string() {
	ifstream f;
	string connection_config_string;
	f.open(ENV_FILE_PATH);
	if (!f.is_open()) {
		cerr << "Failed to find .env file!";
		throw string("Failed to find .env file!");
	}
	getline(f, connection_config_string);
	f.close();

	return connection_config_string;
}


void missing_key_message(int client_socket, const string& missing_key_name) {
	/* For when the client calls a real function but forgets one of its keys */
	json json_message;
	json_message["api_code"] = 400;
	json_message["api_message"] = "Missing expected key: |" + missing_key_name + "|.";
	string client_send_string = json_message.dump();
	client_send_string += '\n';
	send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);
}


int register_new_account(int client_socket, json& client_json) {
	cout << "Triggered register_new_account." << endl;
	string password;
	try {
		password = client_json.at("password");
	}
	catch (json::exception& e) {
		cout << "register_new_account not provided with key 'password'." << endl;
		const string missing_key = "password";
		missing_key_message(client_socket, missing_key);
		return 1;
	}

	cout << "Creating new account with params:" << endl;
	cout << "    password: |" << password << "|" << endl;

	pqxx::connection* psql_connection = establish_psql_connection(get_database_connection_config_string());
	json new_account = register_new_account_in_database(psql_connection, password);
	delete psql_connection;

	cout << "Created new account with:" << endl;
	cout << "    uuid: |" << new_account["uuid"] << "|" << endl;
	cout << "    user_name: |" << new_account["user_name"] << "|" << endl;
	cout << "    tag: |" << new_account["tag"] << "|" << endl;

	cout << "Returning this data to client..." << endl;
	json json_message = new_account;
	json_message["api_code"] = 201;
	json_message["api_message"] = "Created new account.";
	string client_send_string = json_message.dump();
	client_send_string += '\n';
	send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);

	cout << "register_new_account concluded." << endl;

	return 0;
}


int verify_account(int client_socket, json& client_json) {
	cout << "Triggered verify_account." << endl;
	string uuid;
	string password;

	try {
		uuid = client_json.at("uuid");
	}
	catch (json::exception& e) {
		cout << "verify_account missing key 'uuid'." << endl;
		const string missing_key = "uuid";
		missing_key_message(client_socket, missing_key);
		return 1;
	}
	try {
		password = client_json.at("password");
	}
	catch (json::exception& e) {
		cout << "verify_account missing key 'password'." << endl;
		const string missing_key = "password";
		missing_key_message(client_socket, missing_key);
		return 1;
	}

	cout << "Checking authentication for account with:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    password: |" << password << "|" << endl;
	pqxx::connection* psql_connection = establish_psql_connection(get_database_connection_config_string());
	bool valid = verify_existing_account_in_database(psql_connection, uuid, password);
	delete psql_connection;

	json json_message;
	if (valid) {
		cout << "Authorization OK; reporting 200...\n";
		json_message["api_code"] = 200;
		json_message["api_message"] = "Authorization OK.";
	}
	else {
		cout << "Authorization NOT OK. Reporting 401...\n";
		json_message["api_code"] = 401;
		json_message["api_message"] = "Authorization NOT OK.";
	}

	string client_send_string = json_message.dump();
	client_send_string += '\n';
	send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);

	cout << "verify_account concluded." << endl;
	return 0;
}


int change_user_name(int client_socket, json& client_json) {
	cout << "Triggered change_user_name." << endl;
	string uuid;
	string password;
	string new_user_name;

	try {
		uuid = client_json.at("uuid");
	}
	catch (json::exception& e) {
		cout << "change_user_name missing key 'uuid'." << endl;
		const string missing_key = "uuid";
		missing_key_message(client_socket, missing_key);
		return 1;
	}
	try {
		password = client_json.at("password");
	}
	catch (json::exception& e) {
		cout << "change_user_name missing key 'password'." << endl;
		const string missing_key = "password";
		missing_key_message(client_socket, missing_key);
		return 1;
	}
	try {
		new_user_name = client_json["new_user_name"];
	}
	catch (json::exception& e) {
		cout << "change_user_name missing key 'new_user_name'." << endl;
		const string missing_key = "new_user_name";
		missing_key_message(client_socket, missing_key);
		return 1;
	}

	// Authenticate this change.
	pqxx::connection* psql_connection = establish_psql_connection(get_database_connection_config_string());
	bool authorized_change = verify_existing_account_in_database(psql_connection, uuid, password);
	if (!authorized_change) {
		cout << "uuid/password mismatch; cannot perform operation." << endl;
		delete psql_connection;
		json json_message;
		json_message["api_code"] = 401;
		json_message["api_message"] = "Bad uuid/password authorization for this action.";
		string client_send_string = json_message.dump();
		client_send_string += '\n';
		send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);
		return 0;
	}

	cout << "Change request authenticated; commencing." << endl;
	json new_account_info = adjust_user_name_in_database(psql_connection, uuid, new_user_name);

	cout << "New account info is:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;
	cout << "    user_name: |" << new_user_name << "|" << endl;
	cout << "    tag: |" << new_account_info["tag"] << "|" << endl;

	new_account_info["api_code"] = 201;
	new_account_info["api_message"] = "User name has been changed.";
	string client_send_string = new_account_info.dump();
	client_send_string += '\n';
	send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);
	return 0;
}


int z_default(int client_socket, json& fake_client_json) {
	/* We expect client's original message to be in 'message'. */
	string client_message;
	try {
		client_message = fake_client_json["api_message"];
	}
	catch (json::exception& e) {
		cout << "Triggered z_default, but no message was provided?" << endl;

		json json_message;
		json_message["api_code"] = 400;
		json_message["api_message"] = "z_default explicitly called, but provided no message???";
		string client_send_string = json_message.dump();
		send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);
		return 0;
	}

	cout << "Triggered z_default on message |" << client_message << "|" << endl;

	json json_message;
	json_message["api_code"] = 400;
	json_message["api_message"] = "Server does not recognize your request |" + client_message + "|";
	string client_send_string = json_message.dump();
	client_send_string += '\n';
	send(client_socket, client_send_string.c_str(), client_send_string.length(), MSG_NOSIGNAL);
	return 0;
}


int floop(int client_socket, json& client_json) {
	cout << "Triggered floop.";
	json json_message;
	json_message["api_code"] = 200;
	json_message["api_message"] = "You've found the floop function!\n";
	string message_string = json_message.dump();
	message_string += '\n';
	send(client_socket, message_string.c_str(), message_string.length(), MSG_NOSIGNAL);
	return 0;
}


// The different functions, which all take in the socket and json of the message,
//     (which will contain HANDLER_KEY_NAME, their name!) and return an int.
// In the case of no match, or other problems, z_default is called instead with json containing the client's
//     string under the key 'message'.
// Note that you can call z_default normally, and it will be unhappy if you don't provide it a 'message'.
map<string, int (*)(int, json&)> handler_map;


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

		// This is for z_default
		json fake_client_json;
		fake_client_json["api_message"] = client_request;

		json client_json;
		string requested_function_name;
		int (*requested_function)(int, json&);

		// Parse JSON
		try {
			client_json = json::parse(client_request);
		}
		catch (json::exception& e) {
			// Not parsable to json
			cout << "[Server] Did not receive json-y message." << endl;
			handler_map.at(Z_DEFAULT_FUNCTION_NAME)(client_socket, fake_client_json);
			goto label_terminus;
		}
		
		// Extract function name
		try {
			requested_function_name = client_json.at(HANDLER_KEY_NAME);
		}
		catch (json::exception& e) {
			// Did not provide a function name
			cout << "[Server] Did not receive handler key |" << HANDLER_KEY_NAME << "| in request json." << endl;
			handler_map.at(Z_DEFAULT_FUNCTION_NAME)(client_socket, fake_client_json);
			goto label_terminus;
		}

		// Check if requested function is real function
		try {
			requested_function = handler_map.at(requested_function_name);
		}
		catch (out_of_range& oor) {
			// Asked to access non-existent function.
			cout << "[Server] Asked to access non-existent function |" << requested_function_name << "|." << endl;
			handler_map.at(Z_DEFAULT_FUNCTION_NAME)(client_socket, fake_client_json);
			goto label_terminus;
		}

		cout << "[Server] Activating function |" << requested_function_name << "|." << endl;
		requested_function(client_socket, client_json);

		label_terminus:
		cout << "[Server] Closing this connection: server_socket: |" << server_socket << "|, client_socket: |" << client_socket << "|." << endl;
		cout << endl;
		close(client_socket);
	}
	catch (connection_closed) {
		cout << "[Server] The client has closed their connection!" << endl;
		cout << endl;
		return;
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
	handler_map[Z_DEFAULT_FUNCTION_NAME] = z_default;
	handler_map["floop"] = floop;
	handler_map["register_new_account"] = register_new_account;
	handler_map["verify_account"] = verify_account;
	handler_map["change_user_name"] = change_user_name;
	//handler_map[DELETE_ACCOUNT_FUNCTION_NAME] = delete_account;

	listen_for_connections(SERVER_PORT);
	return 0;
}

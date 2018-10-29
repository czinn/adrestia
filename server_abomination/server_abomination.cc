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

// AAHOLAGF:DSJKHFG
string Z_DEFAULT_FUNCTION_NAME("z_default");
string FLOOP_FUNCTION_NAME("000000000000000000000000000floop");
string REGISTER_NEW_ACCOUNT_FUNCTION_NAME("000000000000register_new_account");

#define ENV_FILE_PATH ".env"

#define SALT_LENGTH 16

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
void digest_message(const char* message, size_t message_length, unsigned char** digest_returncarrier, unsigned int* digest_length_returncarrier) {
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
void register_new_account_in_database(pqxx::connection* psql_connection, const string& account_name, const string& password) {
	cout << "Inserting new account |" << account_name << "|:|" << password << "| into database..." << endl;


	const string insert_new_account_into_database_command = ""
	"INSERT INTO adrestia_accounts (account_name, hash_of_salt_and_password, salt)"
	"    VALUES ($1, $2, $3)"
	"    ON CONFLICT DO NOTHING"
	";";

	// Get hash of salt and password
	string salt = hex_urandom(SALT_LENGTH);
	string salt_and_password = salt + password;
	const char* salt_and_password_c_str = salt_and_password.c_str();
	unsigned char* hash_of_salt_and_password = new unsigned char[EVP_MAX_MD_SIZE];
	unsigned int hash_of_salt_and_password_length;
	digest_message(salt_and_password_c_str, salt_and_password.length(), &hash_of_salt_and_password, &hash_of_salt_and_password_length);

	// Insert into table
	psql_connection[0].prepare("insert_new_account_into_database_command", insert_new_account_into_database_command);
	pqxx::work insertion_transaction(psql_connection[0]);
	pqxx::binarystring bstring((void*)(&hash_of_salt_and_password), (size_t)hash_of_salt_and_password_length);
	pqxx::result statement_result = insertion_transaction.prepared("insert_new_account_into_database_command")(account_name)(bstring)(salt).exec();
	insertion_transaction.commit();

	delete hash_of_salt_and_password;

	cout << "Finished insertion of new account into database." << endl;
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

int register_new_account(int client_socket, string recieved_message) {
	cout << "Triggered register_new_account..." << endl;
	auto message_json = json::parse(recieved_message.substr(MESSAGE_HANDLER_NAME_LENGTH, string::npos));
	string account_name = message_json["account_name"];
	string password = message_json["password"];

	cout << "Creating new account |" << account_name << "|:|" << password << "|..." << endl;
	pqxx::connection* psql_connection = establish_psql_connection(get_database_connection_config_string());
	register_new_account_in_database(psql_connection, account_name, password);
	delete psql_connection;

	cout << "register_new_account concluded." << endl;

	return 0;
}

int z_default(int client_socket, string recieved_message) {
	cout << "Triggered z_default on message |" << recieved_message << "|." << endl;
	string message = "Server does not recognize your request |" + recieved_message + "|.\n";
	send(client_socket, message.c_str(), message.length(), MSG_NOSIGNAL);
	return 0;
}

int floop(int client_socket, string recieved_message) {
	cout << "Triggered the floop function." << endl;
	string message = "You've found the floop function!\n";
	send(client_socket, message.c_str(), message.length(), MSG_NOSIGNAL);
	return 0;
}


// The different handler functions, which all take in the socket and message
//     (which will contain the first 32 bytes, their name!) and return an int.
// In case of no match, z_default is called instead. Note that since z_default
//     is less than 32 characters no one can call it normally.
map<string, int (*)(int, string)> handler_map;


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
			handler_map.at(Z_DEFAULT_FUNCTION_NAME)(client_socket, client_request);
		}
		else {
			// We have an actual function name to analyze.
			string requested_function_name = client_request.substr(0, MESSAGE_HANDLER_NAME_LENGTH);
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
				handler_map.at(Z_DEFAULT_FUNCTION_NAME)(client_socket, client_request);
			}
		}
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
	handler_map[FLOOP_FUNCTION_NAME] = floop;
	handler_map[REGISTER_NEW_ACCOUNT_FUNCTION_NAME] = register_new_account;




	listen_for_connections(SERVER_PORT);
	return 0;
}
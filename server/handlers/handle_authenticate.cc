/* Handling requests for 'authenticate'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_authenticate(const string& log_id, const json& client_json, json& resp) {
	/* Checks if a given account/password combination exists in the database.
	 *
	 * Accepts keys from client:
	 *     HANDLER_KEY: <this function>
	 *     "uuid": The uuid to authenticate
	 *     "password": The password
	 *
	 * Responds to client with keys:
	 *     HANDLER_KEY: <this function>
	 *     CODE_KEY: 200 if authentication succeeded; 401 otherwise.
	 *     MESSAGE_KEY: (Message stating authorization succeeded, or did not, as necessary)
	 *
	 * Returns 0 if authentication succeeded, 1 otherwise.
	 */

	cout << "[" << log_id << "] Triggered authenticate." << endl;
	string uuid = client_json.at("uuid");
	string password = client_json.at("password");

	cout << "[" << log_id << "] Checking authentication for account with:" << endl
         << "    uuid: |" << uuid << "|" << endl
	     << "    password: |" << password << "|" << endl;
	pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
	bool valid = adrestia_database::verify_existing_account_in_database(log_id, psql_connection, uuid, password);
	delete psql_connection;

	resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];

	if (valid) {
		resp[adrestia_networking::CODE_KEY] = 200;
		resp[adrestia_networking::MESSAGE_KEY] = "Authorization OK.";
		return 0;
	}
	else {
		resp[adrestia_networking::CODE_KEY] = 401;
		resp[adrestia_networking::MESSAGE_KEY] = "Authorization NOT OK.";
		return 1;
	}

	return 1;
}

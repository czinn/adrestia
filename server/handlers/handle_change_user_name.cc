/* Handling requests for 'change_user_name'. */

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
#include "../../units_cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_change_user_name(const string& log_id, const json& client_json, json& resp) {
	/* Changes the user name associated with the given uuid to the new requested user name.
	 *     By necessity, creates a new tag to go with this user_name.
	 *
	 * Accepts keys from client:
	 *     HANDLER_KEY: <this function>
	 *     "user_name": The new user_name
	 *
	 * Keys inserted by babysitter:
	 *     "uuid": The uuid to modify
	 *
	 * Responds to client with keys:
	 *     HANDLER_KEY: <this function>
	 *     CODE_KEY: 200
	 *     MESSAGE_KEY: "Modification complete."
	 *     "tag": The new tag.
	 *
	 * Always returns 0.
	 */

	cout << "[" << log_id << "] Triggered change_user_name." << endl;
	string uuid = client_json.at("uuid");
	string new_user_name = client_json.at("user_name");

	cout << "[" << log_id << "] Modifying uuid |" << uuid << "| to have user_name |" << new_user_name << "|..." << endl;
	pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
	json new_account_info = adrestia_database::adjust_user_name_in_database(log_id, psql_connection, uuid, new_user_name);

	cout << "[" << log_id << "] New account info is:" << endl
         << "    uuid: |" << uuid << "|" << endl
	     << "    user_name: |" << new_user_name << "|" << endl
	     << "    tag: |" << new_account_info["tag"] << "|" << endl;

	resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
	resp[adrestia_networking::CODE_KEY] = 200;
	resp[adrestia_networking::MESSAGE_KEY] = "Modification complete.";
	resp["tag"] = new_account_info["tag"];

	cout << "[" << log_id << "] change_user_name concluded." << endl;
	return 0;
}

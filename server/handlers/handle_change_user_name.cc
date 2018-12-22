/* Handling requests for 'change_user_name'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_change_user_name(const json& client_json, json& resp) {
	/* Changes the user name associated with the given uuid to the new requested user name.
	 *     By necessity, creates a new tag to go with this user_name.
	 *
	 * Accepts keys from client:
	 *     HANDLER_KEY: <this function>
	 *     "uuid": The uuid to modify
	 *     "user_name": The new user_name
	 *
	 * Responds to client with keys:
	 *     HANDLER_KEY: <this function>
	 *     CODE_KEY: 200
	 *     MESSAGE_KEY: "Modification complete."
	 *     "tag": The new tag.
	 *
	 * Always returns 0.
	 */

	cout << "Triggered change_user_name.

	cout << "Triggered register_new_account." << endl;
	string password = client_json.at("password");

	cout << "Creating new account with params:" << endl;
	cout << "    password: |" << password << "|" << endl;

	pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
	json new_account = adrestia_database::register_new_account_in_database(psql_connection, password);
	delete psql_connection;

	cout << "Created new account with:" << endl;
	cout << "    uuid: |" << new_account["uuid"] << "|" << endl;
	cout << "    user_name: |" << new_account["user_name"] << "|" << endl;
	cout << "    tag: |" << new_account["tag"] << "|" << endl;

	resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
	resp[adrestia_networking::CODE_KEY] = 201;
	resp[adrestia_networking::MESSAGE_KEY] = "Created new account.";
	resp["uuid"] = new_account["uuid"];
	resp["user_name"] = new_account["user_name"];
	resp["tag"] = new_account["tag"];

	cout << "register_new_account concluded." << endl;
	return 0;
}

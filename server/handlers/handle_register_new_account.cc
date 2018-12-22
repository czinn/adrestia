/* Handling requests for 'register_new_account'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_register_new_account(const json& client_json, json &resp) {
	/* Creates a new account in the database.
	 *
	 * Accepts keys from client:
	 *     HANDLER_KEY: <this function>
	 *     "password" The password that the new account will have
	 *
	 * Responds to client with keys:
	 *     HANDLER_KEY: <this function>
	 *     CODE_KEY: 201
	 *     MESSAGE_KEY: "Created new account."
	 *     "uuid": The uuid of the new account
	 *     "user_name": The user_name of the new account
	 *     "tag": The tag of the new account
	 *
	 * Always returns 0.
	 */

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
/* The database interface component for adrestia's networking. */

// Us
#include "adrestia_database.h"

// Our related modules
#include "adrestia_hexy.h"

// Database modules
#include <pqxx/pqxx>

// System modules
#include <cstdlib>  // getenv
#include <iostream>
#include <string>
using namespace std;

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


json adrestia_database::matchmake_in_database (
	pqxx::connection* psql_connection,
	const string& uuid
) {
	/* @brief Adds the user's uuid to the list of uuids waiting for matchmaking, if the user's uuid is not already
	 *        there. If someone else is waiting for matchmaking, and the user can be matched with them, then
	 *        a new game will be made including both players (and the waiter is removed).
	 *
	 * @param psql_connection: The pqxx PostgreSQL connection.
	 * @param uuid: The uuid of the user making the matchmaking request.
	 *
	 * @exception string: In case of failing to create a game - likely due to being unable to generate a
	 *            random game_uid that is not already in use.
	 *
	 * @returns: A json object containing the key "game_uid". If the game_uid is "", then no game was made
	 *           and the given uuid is waiting for matchmaking. Otherwise, the associated value is the game_uid
	 *           of the new game that was made.
	 */

	const string check_for_waiters_command = ""
	"SELECT uuid"
	"    FROM adrestia_match_waiters"
	"    LIMIT 1"
	"    FOR UPDATE"
	";";

	const string insert_waiter_command = ""
	"INSERT INTO adrestia_match_waiters (uuid)"
	"    VALUES ($1)"
	";";

	const string remove_waiter_command = ""
	"DELETE FROM adrestia_match_waiters"
	"    WHERE uuid = $1"
	";";

	const string find_game_uid_command = ""
	"SELECT 1"
	"    FROM adrestia_games"
	"    WHERE game_uid = $1"
	";";

	const string create_game_command = ""
	"INSERT INTO adrestia_games (game_uid, creator_uuid, involved_uuids, activity_state, player_states)"
	"    VALUES ($1, $2, $3, 0, $4)"
	";";

	cout << "matchmake_in_database called with args:" << endl;
	cout << "    uuid: |" << uuid << "|" << endl;

	psql_connection[0].prepare("check_for_waiters_command", check_for_waiters_command);
	psql_connection[0].prepare("insert_waiter_command", insert_waiter_command);
	psql_connection[0].prepare("remove_waiter_command", remove_waiter_command);
	psql_connection[0].prepare("find_game_uid_command", find_game_uid_command);
	psql_connection[0].prepare("create_game_command", create_game_command);

	pqxx::work work_transaction(psql_connection[0]);

	// Check if there are any waiters...
	pqxx::result search_result = work_transaction.prepared("check_for_waiters_command").exec();
	if (search_result.size() == 0) {
		cout << "No possible matches are waiting. We shall become a waiter." << endl;
		work_transaction.prepared("insert_waiter_command")(uuid).exec();

		cout << "Commiting transaction." << endl;
		work_transaction.commit();

		json return_var;
		return_var["game_uid"] = "";
		return return_var;
	}

	string waiting_uuid = search_result[0]["uuid"].as<string>();
	cout << "uuid |" << waiting_uuid << "| is a matching waiter; we will form a new game with them." << endl;

	// Create game id
	string game_uid = "";
	for (int i = 0; i < 1000; i += 1) {
		game_uid = adrestia_hexy::hex_urandom(adrestia_database::GAME_UID_LENGTH);

		pqxx::result game_uid_search_result = work_transaction.prepared("find_game_uid_command")
		                                                               (game_uid)
		                                                               .exec();

		if (game_uid_search_result.size() > 0) {
			continue;
		}
		break;
	}
	if (game_uid.compare("") == 0) {
		cerr << "Failed to create unique game id!" << endl;
		throw string("Failed to create unique game id!");
	}

	string uuid_list = "{'" + uuid + "', '" + waiting_uuid + "'}";
	string states_list = "{0, 0}";

	// Create game
	cout << "Creating game |" << game_uid << "|..." << endl;
	work_transaction.prepared("create_game_command")
	                         (game_uid)(uuid)(uuid_list)(states_list)
	                         .exec();

	// Remove waiting uuid
	cout << "Removing waiting uuid |" << waiting_uuid << "|..." << endl;
	work_transaction.prepared("remove_waiter_command")(waiting_uuid).exec();

	// Commit
	cout << "Committing transaction..." << endl;
	work_transaction.commit();

	json return_var;
	return_var["game_uid"] = game_uid;
	return return_var;
}

json adrestia_database::adjust_user_name_in_database(
	pqxx::connection* psql_connection,
	const string& uuid,
	const string& user_name
) {
	/* @brief Changes all accounts in adrestia_accounts with uuid uuid to have user_name user_name.
	 *        By necessity, this requires the generation of a random tag to go with the name. 1000 attempts will be
	 *        made to generate the tag.
	 *
	 * @param psql_connection: The pqxx PostgreSQL connection.
	 * @param uuid: The uuid whose user_name is to be altered.
	 * @param user_name: The new user_name for the given uuid.
	 *
	 * @exception string: In case of failing to update for some reason - likely due to being unable to generate a
	 *            random tag that is not already in use with the given user_name.
	 *
	 * @returns A json object containing "tag": the new tag generated.
	 */

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
		string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);

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
		catch (pqxx::integrity_constraint_violation &) {
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


json adrestia_database::register_new_account_in_database(
	pqxx::connection* psql_connection,
	const string& password
) {
	/* @brief Creates a new account in adrestia_accounts with the given password. The account will be given a default
	 *        user_name and a random (not-already-in-use-with-this-name) tag to go with it.
	 *        1000 attempts will be made to generate a non-conflicting tag.
	 *
	 * @param psql_connection: The pqxx PostgreSQL connection.
	 * @param password: The password that will be used with this new account.
	 *
	 * @exception string: In the case of failing to generate a non-conflicting tag after 1000 attempts, though possibly
	 *            also in the case of other database integrity constraint violations.
	 *
	 * @returns: A json object with:
	 *               "id": The uuid of the new account
	 *               "user_name": The user_name of the new account
	 *               "tag": The tag of the new account
	 */

	const string insert_new_account_into_database_command = ""
	"INSERT INTO adrestia_accounts (uuid, user_name, tag, hash_of_salt_and_password, salt)"
	"    VALUES ($1, $2, $3, $4, $5)"
	";";

	const string default_user_name = "Initiate";

	cout << "register_new_account_in_database called with args:" << endl;
	cout << "    password: |" << password << "|" << endl;

	// Get hash of salt and password
	string salt = adrestia_hexy::hex_urandom(adrestia_database::SALT_LENGTH);
	string salt_and_password = salt + password;
	const char* salt_and_password_c_str = salt_and_password.c_str();
	unsigned char* hash_of_salt_and_password = new unsigned char[adrestia_hexy::MAX_HASH_LENGTH];
	unsigned int hash_of_salt_and_password_length;
	adrestia_hexy::digest_message(salt_and_password_c_str,
	                              salt_and_password.length(),
	                              &hash_of_salt_and_password,
	                              &hash_of_salt_and_password_length
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
		string uuid = adrestia_hexy::hex_urandom(adrestia_database::UUID_LENGTH);
		string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);

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
		catch (pqxx::integrity_constraint_violation &) {
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


bool adrestia_database::verify_existing_account_in_database(
	pqxx::connection* psql_connection,
	const string& uuid,
	const string& password
) {
	/* @brief Returns true if an account with the given uuid and password exists in the database, false otherwise. */

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
	adrestia_hexy::digest_message(salt_and_password_c_str,
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


pqxx::connection* adrestia_database::establish_psql_connection() {
	/* Returns a pointer to a pqxx::connection in the heap.
	 * Connection parameters specified via environment variable (DB_CONNECTION_STRING)
	 */

	const char *db_conn_string = getenv("DB_CONNECTION_STRING");
	if (db_conn_string == nullptr) {
		cerr << "Failed to read DB_CONNECTION_STRING from env." << endl;
		throw string("Failed to read DB_CONNECTION_STRING from env.");
	}
	pqxx::connection* psql_connection = new pqxx::connection(db_conn_string);
	return psql_connection;
}

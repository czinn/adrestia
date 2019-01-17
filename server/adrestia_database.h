/* The database interface component for adrestia's networking. */

#ifndef ADRESTIA_DATABASE_INCLUDE_GUARD
#define ADRESTIA_DATABASE_INCLUDE_GUARD

// Database modules
#include <pqxx/pqxx>

// System modules
#include <string>

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;

namespace adrestia_database {

	const int SALT_LENGTH = 16;
	const int TAG_LENGTH = 8;
	const int UUID_LENGTH = 32;
	const int GAME_UID_LENGTH = 32;

	/* Fetches from the database the game_state associated with the given game_uid. */
	std::string retrieve_gamestate_from_database(
		const std::string& log_id,
		pqxx::connection* psql_connection,
		const std::string& game_uid
	);

	/* Checks for active games in the database associated with the given uuid, and returns them.
	 *     Also returns if any are waiting for this player's move in particular.
	 */
	json check_for_active_games_in_database (
		const std::string& log_id,
		pqxx::connection* psql_connection,
		const std::string& uuid
	);

	/* Adds the user to the waiting list if there are no compatible waiters;
	 *     otherwise, matches the user to a waiter.
	 */
	json matchmake_in_database(
		const std::string& log_id,
		pqxx::connection* psql_connection,
		const std::string& uuid
	);

	/* Changes the user_name associated with the given uuid in the database.
	 * Returns a json object with key 'tag' representing the tag.
	 */
	json adjust_user_name_in_database(
		const std::string& log_id,
		pqxx::connection* psql_connection,
		const std::string& uuid,
		const std::string& user_name
	);


	/* Creates a new account with a default name in the database.
	 * Returns a json object with keys 'id', 'user_name', and 'tag'.
	 */
	json register_new_account_in_database(
		const std::string& log_id,
		pqxx::connection* psql_connection,
		const std::string& password
	);


	/* Returns true if an account with the given uuid and password exists in the database,
	 *     false otherwise.
	 */
	bool verify_existing_account_in_database(
		const std::string& log_id,
		pqxx::connection* psql_connection,
		const std::string& uuid,
		const std::string& password
	);


	/* Returns an established pqxx::connection object,
	 *     connection parameters specified via environment variable.
	 */
	pqxx::connection* establish_psql_connection();
}

#endif

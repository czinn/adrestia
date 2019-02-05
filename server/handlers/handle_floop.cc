/* Handling requests for 'floop'. */

// Us
#include "../adrestia_networking.h"

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_floop(const Logger& logger, const json& client_json, json& resp) {
	/* Responds with a test message.
	 *
	 * Accepts keys from client:
	 *     HANDLER_KEY: <this function>
	 *
	 * Responds to client with keys:
	 *     HANDLER_KEY: <this function>
	 *     CODE_KEY: 200
	 *     MESSAGE_KEY: "You've found the floop function!"
	 *
	 * Always returns 0.
	 */

	resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
	resp[adrestia_networking::CODE_KEY] = 200;
	resp[adrestia_networking::MESSAGE_KEY] = "floop you too!";

	return 0;
}

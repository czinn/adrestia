#include "../units_cpp/json.h"

using json = nlohmann::json;

void write_error(json &j);
void write_missing_key_error(json &j, const std::string &missing_key_name);

void write_floop_request(json &j);
void write_floop_response(json &j);

void write_register_new_account_request(json &j, const std::string &password);
void write_register_new_account_response(json &j,
	const std::string &uuid,
	const std::string &user_name,
	const std::string &tag);

void write_verify_account_request(json &j,
	const std::string &uuid,
	const std::string &password);
void write_verify_account_response(json &j, bool valid);

void write_change_user_name_request(json &j,
	const std::string &uuid,
	const std::string &password,
	const std::string &new_user_name);
void write_change_user_name_response_unauthorized(json &j);
void write_change_user_name_response(json &j,
	const std::string &tag);

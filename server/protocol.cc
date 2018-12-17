#include "protocol.h"

using json = nlohmann::json;

void write_error(json &j) {
	j["api_code"] = 400;
	j["api_message"] = "Invalid request";
}

void write_missing_key_error(json &j, const std::string &missing_key_name) {
	j["api_code"] = 400;
	j["api_message"] = "Missing expected key: |" + missing_key_name + "|.";
}

void write_floop_request(json &j) {
	j["api_handler_name"] = "floop";
}

void write_floop_response(json &j) {
	j["api_code"] = 200;
	j["api_message"] = "You've found the floop function!\n";
}

void write_register_new_account_request(json &j, const std::string &password) {
	j["api_handler_name"] = "register_new_account";
	j["password"] = password;
}

void write_register_new_account_response(json &j,
	const std::string &uuid,
	const std::string &user_name,
	const std::string &tag)
{
	j["api_handler_name"] = "register_new_account";
	j["api_code"] = 201;
	j["api_message"] = "Created new account.";
	j["uuid"] = uuid;
	j["user_name"] = user_name;
	j["tag"] = tag;
}

void write_verify_account_request(json &j, 
	const std::string &uuid,
	const std::string &password)
{
	j["api_handler_name"] = "verify_account";
	j["uuid"] = uuid;
	j["password"] = password;
}

void write_verify_account_response(json &j, bool valid) {
	j["api_code"] = valid ? 200 : 401;
	j["api_message"] = valid ? "Authorization OK." : "Authorization NOT OK.";
}

void write_change_user_name_request(json &j,
	const std::string &uuid,
	const std::string &password,
	const std::string &new_user_name)
{
	j["api_handler_name"] = "change_user_name";
	j["uuid"] = uuid;
	j["password"] = password;
	j["new_user_name"] = new_user_name;
}

void write_change_user_name_response_unauthorized(json &j) {
	j["api_code"] = 401;
	j["api_message"] = "Bad uuid/password authorization for this action.";
}

void write_change_user_name_response(json &j, const std::string &tag) {
	j["api_code"] = 201;
	j["api_message"] = "User name as been changed.";
	j["tag"] = tag;
}

#include "protocol.h"

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

void write_register_new_account_response(json &j) {
}

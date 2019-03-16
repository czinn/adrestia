#include "adrestia_networking.h"

// Boilerplate for responses.
void adrestia_networking::resp_code(json& resp, int code, const std::string& message) {
  resp[adrestia_networking::CODE_KEY] = code;
  resp[adrestia_networking::MESSAGE_KEY] = message;
}

#pragma once

#include <string>
#include <map>

#include "adrestia_hexy.h"
#include "adrestia_networking.h"

class connection_closed {};
class socket_error {};

// Map from message kinds to functions that handle them. Message kind is stored in field HANDLER_KEY_NAME.
extern std::map<std::string, adrestia_networking::request_handler> handler_map;

class Babysitter {
  public:
    Babysitter(int client_socket);

    void main();

    enum {
      NEW = 0,           // Must establish
      ESTABLISHED = 1,   // Must authenticate or register
      AUTHENTICATED = 2, // You're in!
    } phase;

    int client_socket;
    std::string babysitter_id; // for logging purposes
    std::string read_message_buffer;
    std::string uuid; // The uuid of the client we are babysitting.

    std::string read_message(bool &timed_out);

    void log(const char *format, ...);
};

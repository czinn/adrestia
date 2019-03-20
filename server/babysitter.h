#pragma once

#include <string>
#include <map>
#include <mutex>

#include "adrestia_hexy.h"
#include "adrestia_networking.h"
#include "logger.h"

class connection_closed {};
class socket_error {};

// Map from message kinds to functions that handle them. Message kind is stored in field HANDLER_KEY_NAME.
extern std::map<std::string, adrestia_networking::request_handler> handler_map;

class Babysitter {
  static const long long DISCONNECT_TIMEOUT_MS = 120000;

  // key: uuid
  // value: pointer to the current Babysitter for that user
  static std::map<std::string, Babysitter*> babysitter_for_uuid;
  static std::mutex babysitter_for_uuid_lock;

  enum Phase {
    NEW = 0,           // Must establish
    ESTABLISHED = 1,   // Must authenticate or register
    AUTHENTICATED = 2, // You're in!
  };
  public:
    Babysitter(int client_socket, std::string ip);
    ~Babysitter();

    static void register_for_uuid(const std::string &uuid, Babysitter *babysitter);

    void main();
    Phase phase_new(
        const std::string &endpoint,
        const json &client_json,
        json &resp,
        adrestia_networking::request_handler handler);
    Phase phase_established(
        const std::string &endpoint,
        const json &client_json,
        json &resp,
        adrestia_networking::request_handler handler);
    Phase phase_authenticated(
        const std::string &endpoint,
        json &client_json,
        json &resp,
        adrestia_networking::request_handler handler);

    std::string read_message(bool &timed_out);
    void send_message(const json &j);

    void log(const char *format, ...);

    Phase phase;
    long long last_data_ms;
    int client_socket;
    std::string ip;
    std::string read_message_buffer;
    std::string uuid; // The uuid of the client we are babysitting.
    bool should_die; // Another babysitter has come to replace us
};

#include "babysitter.h"

#include <iostream>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "adrestia_networking.h"
#include "adrestia_database.h"

using namespace std;
using namespace adrestia_networking;

std::map<std::string, adrestia_networking::request_handler> handler_map;

Babysitter::Babysitter(int client_socket)
  :  client_socket(client_socket) {
  const string babysitter_id = adrestia_hexy::hex_urandom(8);
};

void Babysitter::main() {
  /* A single thread lives within this function, babysitting a client's connection.
   *
   * While the individual handler functions may return their own things, in case of error,
   *     expect the following keys:
   *     HANDLER_KEY: generic_error
   *     CODE_KEY: 400
   *     MESSAGE_KEY: <A message describing the problem>
   */

  babysitter_id = adrestia_hexy::hex_urandom(8);
  log("Starting sequence.");

  phase = NEW;
  uuid = "";
  
  // Create pushers
  PushActiveGames push_active_games;
  PushNotifications push_notifications;
  std::vector<Pusher*> pushers = { &push_active_games, &push_notifications };

  try {
    while (true) {
      // Pushers
      if (phase == AUTHENTICATED) {
        log("Processing pushers...");

        for (auto pusher : pushers) {
          for (auto message_json : pusher->push(babysitter_id, uuid)) {
            // We should push the json to the client.
            log("Pushing notification to client.");
            string message_json_string = message_json.dump();
            message_json_string += '\n';
            send(client_socket, message_json_string.c_str(), message_json_string.length(), MSG_NOSIGNAL);
          }
        }
      }

      // read message
      bool timed_out = false;
      string message = read_message(timed_out);

      if (timed_out) {
        continue;
      }

      json client_json;
      json resp;
      string endpoint;
      adrestia_networking::request_handler handler;

      try {
        client_json = json::parse(message);
        endpoint = client_json.at(adrestia_networking::HANDLER_KEY);
        handler = handler_map.at(endpoint);
      } catch (json::parse_error& e) {
        // Not parsable to json
        log("Did not receive json-y message. %s", e.what());
        resp.clear();
        resp[adrestia_networking::HANDLER_KEY] = "generic_error";
        resp[adrestia_networking::CODE_KEY] = 400;
        resp[adrestia_networking::MESSAGE_KEY] = "Could not parse your message as json.";
      } catch (json::out_of_range& e) {
        // Does not contain all required fields
        log("Client json did not have expected key. %s", e.what());
        resp[adrestia_networking::HANDLER_KEY] = "generic_error";
        resp[adrestia_networking::CODE_KEY] = 400;
        resp[adrestia_networking::MESSAGE_KEY] = "Could not find an expected key: |" + string(e.what()) + "|.";
      } catch (std::out_of_range& oor) {
        log("Asked to access non-existent endpoint |%s|", endpoint.c_str());
        resp[adrestia_networking::HANDLER_KEY] = "generic_error";
        resp[adrestia_networking::CODE_KEY] = 400;
        resp[adrestia_networking::MESSAGE_KEY] =
          "Asked to access non-existent endpoint |" + endpoint + "|.";
      }

      try {
        switch (phase) {
          case NEW:
            phase = phase_new(endpoint, client_json, resp, handler);
            break;
          case ESTABLISHED:
            phase = phase_established(endpoint, client_json, resp, handler);
            break;
          case AUTHENTICATED:
            phase = phase_authenticated(endpoint, client_json, resp, handler);
            break;
          default:
            log("We somehow entered an invalid phase.");
        }
      } catch (const string& s) {
        log("Error while running handler: %s", s.c_str());
        resp[adrestia_networking::HANDLER_KEY] = "generic_error";
        resp[adrestia_networking::CODE_KEY] = 400;
        resp[adrestia_networking::MESSAGE_KEY] = "An error occurred.";
      }

      string response_string = resp.dump();
      response_string += '\n';
      send(client_socket, response_string.c_str(), response_string.length(), MSG_NOSIGNAL);
    }
  } catch (connection_closed) {
    log("Client closed the connection.");
  } catch (socket_error) {
    log("Terminating due to socket error.");
  }

  pqxx::connection *conn = adrestia_database::establish_psql_connection();
  adrestia_database::clear_matchmake_requests(this, conn);
  delete conn;

  log("so we should have killed it");
}

Babysitter::Phase Babysitter::phase_new(
    const string &endpoint,
    const json &client_json,
    json &resp,
    adrestia_networking::request_handler handler) {
  if (endpoint == "establish_connection") {
    int valid_connection = handler(babysitter_id, client_json, resp);
    log("Moving to Phase 1.");
    return (valid_connection == 0) ? ESTABLISHED : NEW;
  }
  log("Received out-of-order request for function |%s|.", endpoint.c_str());

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = 400;
  resp[adrestia_networking::MESSAGE_KEY] =
    "out-of-order request for |" + endpoint + "|";
  return NEW;
}

Babysitter::Phase Babysitter::phase_established(
    const string &endpoint,
    const json &client_json,
    json &resp,
    adrestia_networking::request_handler handler) {
  if (endpoint == "register_new_account") {
    handler(babysitter_id, client_json, resp);
    log("Moving to phase 2 via register_new_account.");
    uuid = resp["uuid"];
    return AUTHENTICATED;
  } else if (endpoint == "authenticate") {
    int valid = handler(babysitter_id, client_json, resp);
    if (valid == 0) {
      log("Moving to phase 2 via successful authenticate.");
      uuid = client_json["uuid"];
      return AUTHENTICATED;
    }
    return ESTABLISHED;
  }
  log("Received out-of-order request for function |%s| in phase |%d|.",
      endpoint.c_str(), phase);

  resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
  resp[adrestia_networking::CODE_KEY] = 400;
  resp[adrestia_networking::MESSAGE_KEY] =
    "received out-of-order request for function |" + endpoint + "|";
  return ESTABLISHED;
}

Babysitter::Phase Babysitter::phase_authenticated(
    const string &endpoint,
    json &client_json,
    json &resp,
    adrestia_networking::request_handler handler) {
  client_json["uuid"] = uuid;
  int result = handler(babysitter_id, client_json, resp);
  if (result != 0) {
    log("It seems we got a bad result for that handler: %d", result);
  }
  return AUTHENTICATED;
}

string Babysitter::read_message(bool &timed_out) {
  /* @brief Extracts the message currently waiting on the from the client's socket.
   *        Assumes message is terminated with a \n (if it is not, MESSAGE_MAX_BYTES will be read.
   */
  // TODO: Timeouts
  char buffer[MESSAGE_MAX_BYTES];

  timed_out = false;

  bool complete = read_message_buffer.find('\n') != string::npos;
  while (!complete) {
    int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 2, 0);
    if (bytes_read > 0) {
      for (const char *c = buffer; c < buffer + bytes_read; c++) {
        read_message_buffer += *c;
        if (*c == '\n') complete = true;
      }
    } else if (bytes_read == 0) {
      close(client_socket);
      throw connection_closed();
    } else if (bytes_read == -1) {
      timed_out = true;
      complete = true;
    } else {
      cerr << "Error " << errno << "(" << strerror(errno) << ")" << endl;
      throw socket_error();
    }
  }

  size_t nl_pos = read_message_buffer.find('\n');
  string message = read_message_buffer.substr(0, nl_pos);
  read_message_buffer = read_message_buffer.substr(nl_pos + 1);
  return message;
}

void Babysitter::log(const char *format, ...) {
  va_list args;
  va_start(args, format);
  printf("[%s] ", babysitter_id.c_str());
  vprintf(format, args);
  printf("\n");
  va_end(args);
}

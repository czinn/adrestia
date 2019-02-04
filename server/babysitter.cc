#include "babysitter.h"

#include <iostream>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "adrestia_networking.h"

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
    json client_json;
    json resp;
    
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
      resp.clear();
      bool timed_out = false;
      string message = read_message(timed_out);

      if (timed_out) {
        continue;
      }

      string requested_function_name;

      bool have_valid_function_to_call = true;
      try {
        log("GOT SOMETHING");
        client_json = json::parse(message);
        requested_function_name = client_json.at(adrestia_networking::HANDLER_KEY);
        adrestia_networking::request_handler requested_function = handler_map.at(requested_function_name);

        if ((phase == NEW) && (requested_function_name.compare("establish_connection") != 0)) {
          log("Received out-of-order request for function |%s|.", requested_function_name.c_str());

          resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
          resp[adrestia_networking::CODE_KEY] = 400;
          resp[adrestia_networking::MESSAGE_KEY] =
            "out-of-order request for |" + requested_function_name + "|";
          have_valid_function_to_call = false;
        }
        else if ((phase == ESTABLISHED) &&
            !((requested_function_name.compare("register_new_account") == 0) ||
              (requested_function_name.compare("authenticate") == 0))) {
          log("Received out-of-order request for function |%s| in phase |%d|.",
              requested_function_name.c_str(), phase);

          resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
          resp[adrestia_networking::CODE_KEY] = 400;
          resp[adrestia_networking::MESSAGE_KEY] =
            "received out-of-order request for function |" + requested_function_name + "|";
          have_valid_function_to_call = false;
        }

        if (have_valid_function_to_call) {
          // We have a function that we can actually work with...
          log("Received valid call for function |%s| in phase |%d|.", requested_function_name.c_str(), phase);

          if (requested_function_name.compare("establish_connection") == 0) {
            int valid_connection = requested_function(babysitter_id, client_json, resp);

            if (valid_connection == 0) {
              cout << "[" << babysitter_id << "] moving to phase 1." << endl;;
              phase = ESTABLISHED;
            }
          } else if (requested_function_name.compare("register_new_account") == 0) {
            requested_function(babysitter_id, client_json, resp);

            // This is a type of authentication (and it always succeeds)
            uuid = resp["uuid"];
            cout << "[" << babysitter_id << "] moving to phase 2 via register_new_account." << endl;
            phase = AUTHENTICATED;
          } else if (requested_function_name.compare("authenticate") == 0) {
            int valid_authentication = requested_function(babysitter_id, client_json, resp);

            if (valid_authentication == 0) {
              uuid = client_json["uuid"];
              cout << "[" << babysitter_id << "] moving to phase 2 via successful authenticate." << endl;
              phase = AUTHENTICATED;
            }
          } else {
            // This is all authenticated functions. uuid will be added to client_json.
            client_json["uuid"] = uuid;
            requested_function(babysitter_id, client_json, resp);
          }
        }
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
        log("Asked to access non-existent endpoint |%s|", requested_function_name.c_str());
        resp[adrestia_networking::HANDLER_KEY] = "generic_error";
        resp[adrestia_networking::CODE_KEY] = 400;
        resp[adrestia_networking::MESSAGE_KEY] =
          "Asked to access non-existent endpoint |" + requested_function_name + "|.";
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
    cout << "[" << babysitter_id << "] Client closed the connection." << endl << endl;
  } catch (socket_error) {
    cout << "[" << babysitter_id << "] terminating due to socket error." << endl;
  }
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

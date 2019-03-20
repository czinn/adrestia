#include "babysitter.h"

#include <iostream>
#include <cstdio>
#include <chrono>
#include <mutex>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "adrestia_networking.h"
#include "adrestia_database.h"

#include "pushers/push_active_games.h"
#include "pushers/push_notifications.h"
#include "pushers/push_challenges.h"

#ifdef __APPLE__
#define MSG_NOSIGNAL 0
#endif

using namespace std;
using namespace std::chrono;
using namespace adrestia_networking;

std::map<std::string, request_handler> handler_map;

Babysitter::Babysitter(int client_socket, string ip)
: client_socket(client_socket)
, ip(ip)
, should_die(false) {
  last_data_ms =
    duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
};

std::map<std::string, Babysitter*> Babysitter::babysitter_for_uuid;
std::mutex Babysitter::babysitter_for_uuid_lock;

Babysitter::~Babysitter() {
  adrestia_database::empty_connection_pool();
  babysitter_for_uuid_lock.lock();
  auto current = babysitter_for_uuid.find(uuid);
  if (current->second == this) {
    babysitter_for_uuid.erase(current);
  }
  babysitter_for_uuid_lock.unlock();
}

void Babysitter::register_for_uuid(const string &uuid, Babysitter *babysitter) {
  babysitter_for_uuid_lock.lock();
  auto current = babysitter_for_uuid.find(uuid);
  if (current != babysitter_for_uuid.end()) {
    current->second->should_die = true;
  }
  babysitter_for_uuid[uuid] = babysitter;
  babysitter_for_uuid_lock.unlock();
}

void Babysitter::main() {
  /* A single thread lives within this function, babysitting a client's connection.
   *
   * While the individual handler functions may return their own things, in case of error,
   *     expect the following keys:
   *     HANDLER_KEY: generic_error
   *     CODE_KEY: 400
   *     MESSAGE_KEY: <A message describing the problem>
   */
  adrestia_hexy::reseed();
  logger.prefix = ip;
  logger.info("Connected.");
  phase = NEW;
  uuid = "";

  // Create pushers
  PushActiveGames push_active_games;
  PushNotifications push_notifications;
  PushChallenges push_challenges;
  std::vector<Pusher*> pushers = { &push_active_games, &push_notifications, &push_challenges };

  try {
    while (true) {
      // Pushers
      if (phase == AUTHENTICATED) {
        logger.trace("Processing pushers...");

        for (auto pusher : pushers) {
          for (auto message_json : pusher->push(logger, uuid)) {
            // We should push the json to the client.
            logger.debug("Pushing notification to client.");
            string message_json_string = message_json.dump();
            message_json_string += '\n';
            send(client_socket, message_json_string.c_str(), message_json_string.length(), MSG_NOSIGNAL);
          }
        }
      }

      // read message
      bool timed_out = false;
      string message = read_message(timed_out);

      long long current_time_ms =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

      if (should_die) {
        logger.warn_() << "We've been replaced. Dying." << endl;
        return;
      }
      if (timed_out) {
        if (current_time_ms - last_data_ms > DISCONNECT_TIMEOUT_MS) {
          logger.warn_()
            << "Received no data for "
            << DISCONNECT_TIMEOUT_MS << "ms. "
            "Disconnecting." << endl;
          break;
        }
        continue;
      }
      last_data_ms = current_time_ms;

      json client_json;
      json resp;
      string endpoint;
      request_handler handler;

      try {
        client_json = json::parse(message);
        endpoint = client_json.at(HANDLER_KEY);
        if (endpoint != "floop") {
          if (message.length() < 200) {
            logger.debug_() << "Got message: " << message << endl;
          } else {
            logger.debug_() << "Got message: " << message.substr(0, 197) << "... (full message in trace)" << endl;
            logger.trace_() << message << endl;
          }
        }
        handler = handler_map.at(endpoint);
      } catch (json::parse_error& e) {
        // Not parsable to json
        logger.warn("Did not receive json-y message. %s", e.what());
        resp.clear();
        resp[HANDLER_KEY] = "generic_error";
        resp[CODE_KEY] = 400;
        resp[MESSAGE_KEY] = "Could not parse your message as json.";
      } catch (json::out_of_range& e) {
        // Does not contain all required fields
        logger.warn("Client json did not have expected key. %s", e.what());
        resp[HANDLER_KEY] = "generic_error";
        resp[CODE_KEY] = 400;
        resp[MESSAGE_KEY] = "Could not find an expected key: |" + string(e.what()) + "|.";
      } catch (std::out_of_range& oor) {
        logger.warn("Asked to access non-existent endpoint |%s|", endpoint.c_str());
        resp[HANDLER_KEY] = "generic_error";
        resp[CODE_KEY] = 400;
        resp[MESSAGE_KEY] =
          "Asked to access non-existent endpoint |" + endpoint + "|.";
      }

      if (handler != nullptr) {
        try {
          switch (phase) {
            case NEW:
              phase = phase_new(endpoint, client_json, resp, handler);
              break;
            case ESTABLISHED:
              phase = phase_established(endpoint, client_json, resp, handler);
              // Update tag if phase has changed.
              if (phase == AUTHENTICATED) {
                stringstream username_fc;
                username_fc << resp["user_name"].get<string>() << " (" << resp["friend_code"].get<string>() << ")";
                logger.info_() << "Authenticated as " << username_fc.str() << endl;
                logger.prefix = username_fc.str();
                string uuid = resp["uuid"].get<string>();
                register_for_uuid(uuid, this);
                adrestia_database::Db db;
                db.query(R"sql(
                  INSERT INTO account_ips (uuid, ip)
                  VALUES (?, ?)
                  ON CONFLICT (uuid, ip) DO NOTHING
                )sql")(uuid)(ip)();
                db.commit();
              }
              break;
            case AUTHENTICATED:
              phase = phase_authenticated(endpoint, client_json, resp, handler);
              break;
            default:
              logger.error("We somehow entered an invalid phase.");
          }
        } catch (const string& s) {
          logger.error("Error while running handler: %s", s.c_str());
          resp[HANDLER_KEY] = "generic_error";
          resp[CODE_KEY] = 400;
          resp[MESSAGE_KEY] = "An error occurred.";
        }
      }

      send_message(resp);
    }
  } catch (connection_closed) {
    logger.info("Client closed the connection.");
  } catch (socket_error) {
    logger.error("Terminating due to socket error.");
  } catch (json::exception &e) {
    logger.error_()
      << "Had a JSON exception" << endl
      << e.what() << endl;
  } catch (std::exception &e) {
    logger.error_()
      << "Had an unknown exception" << endl
      << e.what() << endl;
  }

  adrestia_database::Db db(logger);
  db.query(R"sql(
    DELETE FROM adrestia_match_waiters
    WHERE uuid = ?
  )sql")(uuid)();
  db.query(R"sql(
    DELETE FROM challenges
    WHERE sender_uuid = ?
  )sql")(uuid)();
  db.query(R"sql(
    UPDATE adrestia_accounts
    SET is_online = false
    WHERE uuid = ?
  )sql")(uuid)();
  db.commit();
}

Babysitter::Phase Babysitter::phase_new(
    const string &endpoint,
    const json &client_json,
    json &resp,
    request_handler handler) {
  if (endpoint == "establish_connection") {
    int valid_connection = handler(logger, client_json, resp);
    logger.trace("Moving to phase 1.");
    return (valid_connection == 0) ? ESTABLISHED : NEW;
  }
  logger.warn("Received out-of-order request for function |%s|.", endpoint.c_str());

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp[CODE_KEY] = 400;
  resp[MESSAGE_KEY] =
    "out-of-order request for |" + endpoint + "|";
  return NEW;
}

Babysitter::Phase Babysitter::phase_established(
    const string &endpoint,
    const json &client_json,
    json &resp,
    request_handler handler) {

  if (endpoint == "register_new_account") {
    handler(logger, client_json, resp);
    logger.trace("Moving to phase 2 via register_new_account.");
    uuid = resp["uuid"];
    return AUTHENTICATED;
  } else if (endpoint == "authenticate") {
    int valid = handler(logger, client_json, resp);
    if (valid == 0) {
      logger.trace("Moving to phase 2 via successful authenticate.");
      uuid = client_json["uuid"];
      return AUTHENTICATED;
    }
    return ESTABLISHED;
  }
  logger.warn("Received out-of-order request for function |%s| in phase |%d|.",
      endpoint.c_str(), phase);

  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp[CODE_KEY] = 400;
  resp[MESSAGE_KEY] =
    "received out-of-order request for function |" + endpoint + "|";
  return ESTABLISHED;
}

Babysitter::Phase Babysitter::phase_authenticated(
    const string &endpoint,
    json &client_json,
    json &resp,
    request_handler handler) {
  client_json["uuid"] = uuid;

  int result = handler(logger, client_json, resp);
  if (result != 0) {
    logger.warn("It seems we got a bad result for that handler: %d", result);
  }
  return AUTHENTICATED;
}

string Babysitter::read_message(bool &timed_out) {
  /* Extracts the message currently waiting on the from the client's socket.
   * Assumes message is terminated with a \n (if it is not, MESSAGE_MAX_BYTES
   * will be read.)
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

void Babysitter::send_message(const json &j) {
  string response_string = j.dump() + '\n';
  send(client_socket, response_string.c_str(), response_string.length(), MSG_NOSIGNAL);
}

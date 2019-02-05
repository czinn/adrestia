/* Pushes notifications */

// Us
#include "push_notifications.h"

// Our related modules
#include "../adrestia_networking.h"
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <algorithm> // Find
#include <iostream>
#include <set>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

adrestia_networking::PushNotifications::PushNotifications() {}

std::vector<json> adrestia_networking::PushNotifications::push(const Logger& logger, const string& uuid) {
  /* @brief Returns a list of messages for any new notifications
   *
   * @param logger: Logger
   * @param uuid: The uuid for which notifications will be checked.
   *
   * @sideeffect: latest_notification_already_sent will be updated to the ID of
   * the last notification sent. Only notifications with a greater ID will be
   * sent in the future.
   *
   * @returns: If there are new notifications, the result will contain
   * messages with the following keys:
   *   HANDLER_KEY: "push_notifications"
   *   CODE_KEY: 200
   *   MESSAGE_KEY: "New notification"
   *   "message": Message to display to user
   */
  pqxx::connection psql_connection = adrestia_database::establish_connection();

  std::vector<std::string> notifications = adrestia_database::get_notifications(
      logger, psql_connection, uuid, latest_notification_already_sent);

  std::vector<json> result;
  for (const std::string &message : notifications) {
    json message_json;
    message_json[adrestia_networking::HANDLER_KEY] = "push_notifications";
    message_json[adrestia_networking::CODE_KEY] = 200;
    message_json[adrestia_networking::MESSAGE_KEY] = "New notification";
    message_json["message"] = message;
    result.push_back(message_json);
  }

  return result;
}

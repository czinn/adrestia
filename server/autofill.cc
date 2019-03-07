// Adrestia
#include "adrestia_networking.h"
#include "adrestia_hexy.h"

// Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <random>
#include <chrono>
#ifdef __APPLE__
#include <sys/wait.h>
#define MSG_NOSIGNAL 0
#else
#include <wait.h>
#endif
#include <unistd.h>

// System modules
#include <fstream>
#include <iostream>
#include <list>
#include <string>
using namespace std;

#include "../cpp/cfr_strategy.h"

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;


string SERVER_IP("127.0.0.1");


class connection_closed {};
class socket_error {};

string hex_urandom(unsigned int number_of_characters) {
  // Creates random hexadecimal string of requested length
  char proto_output[number_of_characters + 1];

  ifstream urandom("/dev/urandom", ios::in|ios::binary);

  if (!urandom) {
    cerr << "Failed to open urandom!\n";
    throw;
  }

  for (unsigned int i = 0; i < number_of_characters; i = i + 1) {
    char next_number = 0;

    urandom.read((char*)(&next_number), sizeof(char));
    next_number &= 0x0F;  // Ensure we generate only one character at a time.

    if (!urandom) {
      cerr << "Failed to read from urandom!\n";
      throw;
    }

    if (next_number < 10) {  // 0-10
      proto_output[i] = (char)(next_number + 48);
    }
    else {  // A-F
      proto_output[i] = (char)(next_number + 87);
    }
  }

  proto_output[number_of_characters] = '\0';

  string returnVar(proto_output);
  return returnVar;
}

string recv_buffer;

string read_packet (int client_socket) {
  // Reads a string sent from the target.
  // The string should end with a newline, although this newline is not returned by this function.
  const int size = 8192;
  char buffer[size];

  while (recv_buffer.find('\n') == string::npos)
  {
    int bytes_read = recv (client_socket, buffer, sizeof(buffer) - 1, 0);
      // Though extremely unlikely in our setting --- connection from
      // localhost, transmitting a small packet at a time --- this code
      // takes care of fragmentation  (one packet arriving could have
      // just one fragment of the transmitted message)

    if (bytes_read > 0)
    {
      buffer[bytes_read] = '\0';
      recv_buffer += buffer;
      if (strchr(buffer, '\n') != nullptr) {
        break;
      }
    }

    else if (bytes_read == 0)
    {
      close (client_socket);
      throw connection_closed();
    }

    else
    {
      cerr << "Error " << errno << " (" << strerror(errno) << ")" << endl;
      throw socket_error();
    }
  }

  size_t newline_index = recv_buffer.find('\n');
  string message = recv_buffer.substr(0, newline_index);
  recv_buffer = recv_buffer.substr(newline_index + 1);
  return message;
}

list<json> packet_cache;

json read_packet(int client_socket, string handler_key) {
  // Check the packet cache first
  for (auto it = packet_cache.begin(); it != packet_cache.end(); it++) {
    if ((*it)[adrestia_networking::HANDLER_KEY] == handler_key) {
      json packet = *it;
      packet_cache.erase(it);
      return packet;
    }
  }
  // If that doesn't give you a packet, keep reading packets until we get one with the right handler key
  while (true) {
    string packet_str = read_packet(client_socket);
    json packet = json::parse(packet_str);
    if (packet[adrestia_networking::HANDLER_KEY] == handler_key) {
      return packet;
    }
    packet_cache.push_back(packet);
  }
}


int socket_to_target(const char* IP, int port) {
  // Creates and connects socket to target IP/port
  struct sockaddr_in target_address;

  target_address.sin_family = AF_INET;
  target_address.sin_addr.s_addr = inet_addr(IP);
  target_address.sin_port = htons(port);

  int my_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (connect(my_socket, (struct sockaddr*)&target_address, sizeof(target_address)) == -1) {
    cout << "Error establishing connection: |" << strerror(errno) << "|.\n";
    return -1;
  }

  return my_socket;
}

int main(int argc, char* argv[]) {  
  std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

  string my_uuid;
  string my_user_name;
  string my_tag;
  string version = version_to_string(adrestia_networking::CLIENT_VERSION);
  string password("signal-expect-surprise-quickly");
  string desired_user_name1("asdf;lkj"); // TODO: generate a realistic username if usernames are displayed to users

  json outbound_json;
  json response_json;
  string outbound_message;

  const char* server_port_env = getenv("SERVER_PORT");
  int port = adrestia_networking::DEFAULT_SERVER_PORT;
  if (server_port_env) {
    port = atoi(server_port_env);
  }

  // Establish connection (socket)
  int my_socket_1 = socket_to_target(SERVER_IP.c_str(), port);
  if (my_socket_1 == -1) {
    cerr << "Failed to establish connection (socket)." << endl;
    return 0;
  }

  // Establish connection (endpoint)
  outbound_json.clear();
  response_json.clear();

  adrestia_networking::create_establish_connection_call(outbound_json, version);
  outbound_message = outbound_json.dump() + '\n';
  send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
  response_json = read_packet(my_socket_1, "establish_connection");
  if (response_json[adrestia_networking::CODE_KEY] != 200) {
    cerr << "Failed to establish connection (endpoint)." << endl;
    cerr << "establish_connection says:" << endl;
    cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
    cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
    cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
    close(my_socket_1);
    return 0;
  }
  GameRules rules;
  rules = response_json["game_rules"];

  // Create account
  outbound_json.clear();
  response_json.clear();

  adrestia_networking::create_register_new_account_call(outbound_json, password, true);
  outbound_message = outbound_json.dump() + '\n';
  send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
  response_json = read_packet(my_socket_1, "register_new_account");
  if (response_json[adrestia_networking::CODE_KEY] != 201) {
    cerr << "Failed to register new account." << endl;
    cerr << "register_new_account says:" << endl;
    cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
    cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
    cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
    close(my_socket_1);
    return 0;
  }
  else {
    my_uuid = response_json["uuid"];
    my_user_name = response_json["user_name"];
    my_tag = response_json["tag"];
  }

  // Select random books
  vector<string> selected_books;
  for (const auto &[id, book] : rules.get_books()) {
    if (selected_books.size() < 3) {
      selected_books.push_back(id);
    } else {
      int index = gen() % 3;
      if (index < 3) {
        selected_books[index] = id;
      }
    }
  }

  // Matchmake
  outbound_json.clear();
  response_json.clear();

  adrestia_networking::create_matchmake_me_call(outbound_json, rules, selected_books);
  outbound_message = outbound_json.dump() + '\n';
  send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
  response_json = read_packet(my_socket_1, "matchmake_me");
  if (response_json[adrestia_networking::CODE_KEY] == 200) {
    cout << "No players waiting. Disconnecting." << endl;
    close(my_socket_1);
    return 0;
  } else if (response_json[adrestia_networking::CODE_KEY] != 201) {
    cout << "Failed to make a match." << endl;
    cerr << "matchmake_me says:" << endl;
    cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
    cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
    cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
    close(my_socket_1);
    return 0;
  }

  CfrStrategy strategy(rules, 10001);

  while (true) {
    response_json = read_packet(my_socket_1, "push_active_games");
    if (response_json[adrestia_networking::CODE_KEY] != 200) {
      cerr << "Recieved error instead of game update." << endl;
      cerr << "Received instead:" << endl;
      cerr << "    HANDLER: |" << response_json[adrestia_networking::HANDLER_KEY] << "|" << endl;
      cerr << "    CODE: |" << response_json[adrestia_networking::CODE_KEY] << "|" << endl;
      cerr << "    MESSAGE: |" << response_json[adrestia_networking::MESSAGE_KEY] << "|" << endl;
      close(my_socket_1);
      return 0;
    }

    json game = response_json["updates"][0];

    if (game.find("game_state") != game.end()) {
      // Game is over
      break;
    }

    GameView view(rules, game["game_view"]);

    // Compute and submit a move
    GameAction action = strategy.get_action(view);
    cout << json(action) << endl;
    outbound_json.clear();
    adrestia_networking::create_submit_move_call(outbound_json, game["game_uid"], action);
    outbound_message = outbound_json.dump() + '\n';
    send(my_socket_1, outbound_message.c_str(), outbound_message.length(), MSG_NOSIGNAL);
    response_json = read_packet(my_socket_1, "submit_move");
    if (response_json[adrestia_networking::CODE_KEY] != 200) {
      cerr << "Server rejected a legal move." << endl;
      cerr << json(response_json) << endl;
    }
  }

  close(my_socket_1);

  return 0;
}

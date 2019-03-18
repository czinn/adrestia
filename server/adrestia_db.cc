#include "../cpp/game_rules.h"
#include "../cpp/game_state.h"

#include "adrestia_database.h"
#include "logger.h"

using namespace adrestia_database;

thread_local std::vector<pqxx::connection*> adrestia_database::connection_pool;

void adrestia_database::empty_connection_pool() {
  while (!connection_pool.empty()) {
    pqxx::connection *conn = connection_pool.back(); connection_pool.pop_back();
    delete conn;
  }
}

DbQuery::DbQuery(std::string format, pqxx::work *work)
  : work(work)
  , has_run(false)
{
  // split format by ?s
  size_t cur, prev = 0;
  while (true) {
    cur = format.find('?', prev);
    format_parts.push_back(format.substr(prev, cur - prev));
    if (cur == std::string::npos) break;
    prev = cur + 1;
  }
}

DbQuery::~DbQuery() {
  if (!has_run) {
    logger.warn_()
      << "Warning: The following query was constructed but not actually run.\n"
      << build_query()
      << "Did you forget to call operator()?\n"
      << std::endl;
  }
}

std::string DbQuery::build_query() {
  std::stringstream query_builder;
  for (size_t i = 0; i < format_parts.size() - 1; i += 1) {
    query_builder << format_parts[i];
    query_builder << ((i < quoted_parts.size()) ? quoted_parts[i] : "?");
  }
  query_builder << format_parts.back();
  return query_builder.str();
}

pqxx::result DbQuery::operator()() {
  has_run = true;
  if (format_parts.size() != quoted_parts.size() + 1) {
    throw std::string("Wrong number of arguments supplied to SQL query.");
  }
  std::string query = build_query();
  logger.trace_() << "Running SQL: " << query << std::endl;
  return work->exec(query);
}

Db::Db() {
  if (connection_pool.empty()) {
    const char *db_conn_string = getenv("DB_CONNECTION_STRING");
    if (db_conn_string == nullptr) {
      logger.error_() << "Failed to read DB_CONNECTION_STRING from env." << std::endl;
      throw std::string("Failed to read DB_CONNECTION_STRING from env.");
    }
    connection_pool.push_back(new pqxx::connection(db_conn_string));
  }
  conn = connection_pool.back(); connection_pool.pop_back();
  work = new pqxx::work(*conn);
}

// Dummy that discards the logger, to ease transition to thread_local logger.
Db::Db(const Logger &logger) : Db() { }

Db::~Db() {
  delete work;
  connection_pool.push_back(conn);
}

DbQuery Db::query(std::string format) {
  return DbQuery(format, work);
}

void Db::commit() {
  logger.trace("Committing transaction.");
  work->commit();
  delete work;
  work = new pqxx::work(*conn);
}

void Db::abort() {
  logger.trace("Aborting transaction.");
  work->abort();
  delete work;
  work = new pqxx::work(*conn);
}

GameRules Db::retrieve_game_rules(int id) {
  auto result =
    (id == 0)
    ? query("SELECT game_rules FROM adrestia_rules ORDER BY -id LIMIT 1")()
    : query("SELECT game_rules FROM adrestia_rules WHERE id = ?")(id)();
  if (result.size() == 0) {
    throw std::string("Could not find game rules. Do they exist?");
  }
  return json::parse(result[0][0].as<std::string>());
}

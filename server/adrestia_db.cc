#include "adrestia_database.h"

using namespace adrestia_database;

DbQuery::DbQuery(std::string format, pqxx::work *work, const Logger &logger)
  : work(work)
  , logger(logger)
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

pqxx::result DbQuery::operator()() {
  if (format_parts.size() != quoted_parts.size() + 1) {
    throw std::string("Wrong number of arguments supplied to SQL query.");
  }
  std::stringstream query_builder;
  for (size_t i = 0; i < quoted_parts.size(); i += 1) {
    query_builder << format_parts[i];
    query_builder << quoted_parts[i];
  }
  query_builder << format_parts.back();
  std::string query = query_builder.str();
  logger.trace_() << query << std::endl;
  return work->exec(query);
}

Db::Db(const Logger &logger)
  : logger(logger)
{
  const char *db_conn_string = getenv("DB_CONNECTION_STRING");
  if (db_conn_string == nullptr) {
    logger.error_() << "Failed to read DB_CONNECTION_STRING from env." << std::endl;
    throw std::string("Failed to read DB_CONNECTION_STRING from env.");
  }

  // TODO: jim: Take from a pool
  conn = new pqxx::connection(db_conn_string);
  work = new pqxx::work(*conn);
}

Db::~Db() {
  delete work;
  delete conn;
}

DbQuery Db::query(std::string format) {
  return DbQuery(format, work, logger);
}

void Db::commit() {
  work->commit();
  delete work;
  work = new pqxx::work(*conn);
}

void Db::abort() {
  work->abort();
  delete work;
  work = new pqxx::work(*conn);
}

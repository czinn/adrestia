#include "adrestia_database.h"

using namespace adrestia_database;

DbQuery::DbQuery(std::string format, pqxx::work *work, const Logger &logger)
  : format(format)
  , work(work)
  , logger(logger)
{}

pqxx::result DbQuery::operator()() {
  if (format.find('?') != std::string::npos) {
    throw std::string("Too few arguments supplied to SQL query.");
  }
  logger.trace_() << format << std::endl;
  return work->exec(format);
}

void DbQuery::replace_one_qmark(std::string s) {
  logger.error_() << format << std::endl;
  logger.error_() << s << std::endl;
  size_t pos = format.find('?');
  if (pos == std::string::npos) {
    throw std::string("Too many arguments supplied to SQL query.");
  }
  format.replace(pos, 1, s);
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

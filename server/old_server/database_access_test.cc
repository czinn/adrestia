#include <iostream>
#include <fstream>
#include <string>
#include <pqxx/pqxx>
using namespace std;

const string env_file_name = ".env";
// Should contain a single line like this:
// host=localhost port=5432 dbname=<database> user=<user> password='<password>'

int main(int argv, char** argc) {
    const string insert_test_string_command = ""
    "INSERT INTO test_table (test_string)"
    "VALUES ($1)"
    "ON CONFLICT DO NOTHING"
    ";";

    ifstream f;
    string database_access_string;
    f.open(env_file_name.c_str());
    if (!f.is_open()) {
        cerr << "Failed to find .env file!" << endl;
        return 1;
    }
    getline(f, database_access_string);
    f.close();

    try {
        pqxx::connection psql_connection(database_access_string);

        psql_connection.prepare("insert_test_string_command", insert_test_string_command);

        pqxx::work test_transaction(psql_connection);
        pqxx::result statement_result = test_transaction.prepared("insert_test_string_command")("mah test string").exec();
        test_transaction.commit();
    }
    catch (const std::exception &e) {
        cerr << "ERROR: |" << e.what() << "|" << endl;
        return 1;
    }

    return 0;
}

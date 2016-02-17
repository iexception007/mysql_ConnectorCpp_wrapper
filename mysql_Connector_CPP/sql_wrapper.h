/*
 * sql_wrapper.h
 *
 *  Created on: Feb 16, 2016
 *  Author: Xiaofeng.Wang
 */

#ifndef FTRD_DB_SQL_WRAPPER_H_
#define FTRD_DB_SQL_WRAPPER_H_

/* MySQL Connector/C++ API */
#include "mysql_connection.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "connpool.h"

class Wrapper {
public:
    Wrapper();
    ~Wrapper();
    bool switchDb(const std::string &db_name);
    bool prepare(const std::string &query);
    void release_connection();
    int get_currentConnection();
    void setInt(const int num, const int data);
    void setString(const int num, const std::string &data);
    bool execute(const std::string &query = "");
    bool fetch();
    std::string print(const std::string &field);
    std::string print(const int index);
private:
    ConnectionPool *cpool_;
    sql::Connection *conn_;
    sql::Statement *stmt_;
    sql::PreparedStatement *prep_stmt_;
    sql::ResultSet *res_;
};



#endif /* FTRD_DB_SQL_WRAPPER_H_ */

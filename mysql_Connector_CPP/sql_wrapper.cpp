/*
 * sql_wrapper.cpp
 *
 *  Created on: Feb 16, 2016
 *  Author: Xiaofeng.Wang
 */

#include "sql_wrapper.h"

using std::cout;
using std::endl;
using std::string;
using namespace sql;

Wrapper::Wrapper() : stmt_(nullptr), prep_stmt_(nullptr), res_(nullptr),
        conn_(nullptr), cpool_(nullptr)
{
    cpool_ = ConnectionPool::GetInstance();
}

Wrapper::~Wrapper()
{
    delete stmt_;
    delete prep_stmt_;
    delete res_;
    delete cpool_;
}

void Wrapper::release_connection()
{
    cpool_->ReleaseConnection(conn_);  //释放连接到池中
}

int Wrapper::get_currentConnection()
{
    return cpool_->GetCurrentSize();
}

bool Wrapper::switchDb(const string &db_name)
{
    conn_ = cpool_->GetConnection();

    if (conn_ == nullptr) {
        cout << "fail to GetConnection" << endl;
        return false;
    }

    try {
        conn_->setSchema(db_name);
        stmt_ = conn_->createStatement();
    } catch (sql::SQLException &e) {
        ERROR(e);
        return false;
    }

    return true;
}

bool Wrapper::prepare(const string &query)
{
    try {
        prep_stmt_ = conn_->prepareStatement(query);
    } catch (sql::SQLException &e) {
        ERROR(e);
        return false;
    }

    return true;
}

void Wrapper::setInt(int num, int data)
{
    prep_stmt_->setInt(num, data);
}

void Wrapper::setString(const int num, const string &data)
{
    prep_stmt_->setString(num, data);
}

bool Wrapper::execute(const string &query)
{
    try {
        if (query != "") {
            res_ = stmt_->executeQuery(query);
        } else {
            res_ = prep_stmt_->executeQuery();
            prep_stmt_->close();
        }
    } catch (sql::SQLException &e) {
        ERROR(e);
        return false;
    }

    return true;
}

bool Wrapper::fetch()
{
    return res_->next();
}

string Wrapper::print(const string &field)
{
    return res_->getString(field);
}

string Wrapper::print(int index)
{
    return res_->getString(index);
}

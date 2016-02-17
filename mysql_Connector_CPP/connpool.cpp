/*
 * connpool.cpp
 *
 *  Created on: Jan 29, 2016
 *  Author: Xiaofeng.Wang
 */

#include <mutex>
#include "connpool.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace sql;

ConnectionPool *ConnectionPool::connpool_ = nullptr;
std::once_flag once;

ConnectionPool::ConnectionPool(string hostname, string username,
        string password, int maxsize) :
        hostname_(hostname), username_(username), password_(password), maxsize_(
                maxsize), currsize_(0), driver_(nullptr)
{
    try {
        this->driver_ = get_driver_instance();
    } catch (sql::SQLException &e) {
        ERROR(e);
    }

    pthread_mutex_init(&tlock_, NULL);

    this->InitConnection(INITIAL_CONN_SIZE);
}

ConnectionPool::~ConnectionPool()
{
    this->DestroyConnPool();
    pthread_mutex_destroy(&tlock_);
}

Connection *ConnectionPool::CreateConnection()
{
    Connection *conn_ = nullptr;

    try {
        conn_ = driver_->connect(hostname_, username_, password_);
        return conn_;
    } catch (sql::SQLException &e) {
        ERROR(e);
        return nullptr;
    }
}

void ConnectionPool::InitConnection(int initial_size)
{
    Connection *conn_ = nullptr;

    for (int i = 0; i < initial_size; i++) {
        conn_ = this->CreateConnection();
        if (conn_) {
            connlist_.push_back(conn_);
            ++(this->currsize_);
        } else {
            cout << "Create connection error!!" << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void ConnectionPool::DestroyConnection(sql::Connection *conn)
{
    if (conn) {
        try {
            conn->close();
        } catch (sql::SQLException &e) {
            ERROR(e);
        }
        delete conn;
    }
}

void ConnectionPool::DestroyConnPool()
{
    vector<sql::Connection *>::iterator iter;

    for (iter = connlist_.begin(); iter != connlist_.end(); ++iter)
        this->DestroyConnection(*iter);

    this->currsize_ = 0;
    connlist_.clear();
}

Connection *ConnectionPool::GetConnection()
{
    Connection *conn_ = nullptr;

    pthread_mutex_lock(&tlock_);

    //若池中还有连接
    if (connlist_.size() > 0) {
        conn_ = connlist_.back();
        connlist_.pop_back();
        //如果已经关闭，那么就删除重建
        if (conn_->isClosed()) {
            delete conn_;
            conn_ = nullptr;
            conn_ = this->CreateConnection();
        }
        //如果连接为空，则创建失败，将当前连接数减1
        if (nullptr == conn_)
            --currsize_;

        pthread_mutex_unlock(&tlock_);
        return conn_;
    } else {
        //连接数未达到上限, 继续创建新的连接
        if (currsize_ < maxsize_) {
            conn_ = this->CreateConnection();
            if (conn_) {
                ++currsize_;
                pthread_mutex_unlock(&tlock_);
                return conn_;
            } else {
                pthread_mutex_unlock(&tlock_);
                return nullptr;
            }
        } else {
            //连接数达到上限
            pthread_mutex_unlock(&tlock_);
            return nullptr;
        }
    }
}

ConnectionPool *ConnectionPool::GetInstance()
{
    std::call_once(once, []() {connpool_ = new ConnectionPool(
                HOSTNAME, USERNAME, PASSWORD, MAX_CONN_SIZE);});
    return connpool_;
}

void ConnectionPool::ReleaseConnection(Connection *connection)
{
    if (connection) {
        pthread_mutex_lock(&tlock_);
        connlist_.push_back(connection);
        pthread_mutex_unlock(&tlock_);
    }
}

int ConnectionPool::GetCurrentSize(void)
{
    return currsize_;
}

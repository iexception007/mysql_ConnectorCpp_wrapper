/*
 * connpool.h
 *
 *  Created on: Jan 29, 2016
 *  Author: Xiaofeng.Wang
 *  @brief: ConnectionPool for MySQL Connector/C++
 */

#ifndef FTRD_DB_CONNPOOL_H_
#define FTRD_DB_CONNPOOL_H_

/* Standard C/C++ Includes */
#include <iostream>
#include <string.h>
#include <vector>
#include <pthread.h>

/* MySQL Connector/C++ API */
#include <cppconn/driver.h>

#define MAX_CONN_SIZE    5
#define INITIAL_CONN_SIZE	 2
#define HOSTNAME    "localhost"
#define USERNAME    "root"
#define PASSWORD    "123456"

#define ERROR(_exception_)	do {  \
			cout << "# ERR: SQLException in " << __FILE__;	\
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;	\
			cout << "# ERR: " << _exception_.what();	\
			cout << " (MySQL error code: " << _exception_.getErrorCode();	\
			cout << ", SQLState: " << _exception_.getSQLState() << " )" << endl;	\
		} while (0)

class ConnectionPool {
private:
    ConnectionPool(std::string hostname, std::string username,
            std::string password, int maxsize);

    sql::Connection *CreateConnection(); //创建连接
    void InitConnection(int initial_size);  //初始化连接池
    void DestroyConnection(sql::Connection *conn);  //销毁连接对象
    void DestroyConnPool(); //销毁连接池

public:
    ~ConnectionPool();
    ConnectionPool(const ConnectionPool&) = delete;            //阻止拷贝
    ConnectionPool& operator=(const ConnectionPool&) = delete; //阻止赋值

    sql::Connection* GetConnection();   //获得数据库连接
    void ReleaseConnection(sql::Connection *conn);  //将数据库连接释放到连接池
    static ConnectionPool *GetInstance();   //获取连接对象
    int GetCurrentSize(void);   //返回当前连接数

private:
    int currsize_;  //当前已建立的连接数
    int maxsize_;   //连接池中定义的最大连接数
    std::string hostname_;
    std::string username_;
    std::string password_;
    pthread_mutex_t tlock_;

    static ConnectionPool* connpool_;
    std::vector<sql::Connection*> connlist_;    //连接池容器
    sql::Driver *driver_;
};

#endif /* FTRD_DB_CONNPOOL_H_ */

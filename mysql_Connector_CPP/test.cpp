/*
 * test.cpp
 *
 *  Created on: Jan 29, 2016
 *  Author: Xiaofeng.Wang
 */
#include <iostream>
#include <memory>

#include "sql_wrapper.h"

using namespace std;
using namespace sql;

const char *sql_query = "select stu_name, stu_score from student_info where stu_name = 'yck'";

int main(int argc, char **argv)
{
    shared_ptr<Wrapper> wp = make_shared<Wrapper>();

    if (!wp->switchDb("student")) {
        cout << "switchDb error" << endl;
        return -1;
    }

  /*  if (!wp->execute(sql_query)) {
        cout << "fail to exec" << endl;
        return -1;
    }*/
    if (!wp->prepare(sql_query)) {
        cout << "prepare error" << endl;
        return -1;
    }
    wp->execute();

    while (wp->fetch())
        cout << "name = " << wp->print(1) << ", score = " << wp->print(2) << endl;

    wp->release_connection();

    return 0;
}

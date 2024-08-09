#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <set>

#include <pqxx/pqxx>

#include "user.h"
#include "task.h"

class Database {
public:
    static Database& get_instance() {
        static Database instance;
        return instance;
    }
    void load_tables();
    void add_user(const std::string& username);
    void add_task(const Task& task, const std::set<int>& collaborators_id);
    void assign_users_to_task(int task_id, int creator_id, const std::set<int>& collaborators_id);

    std::vector<Task> get_tasks_for_user(int user_id);
    Task get_task_by_id(int task_id);

    std::vector<User> get_collaborators_for_task(int task_id);
    User get_user_by_name(const std::string& username);
    User get_user_by_id(int user_id);
    bool user_exists(const std::string& username);

    void connect();
private:
    Database();
    std::string connection_data;
    std::unique_ptr<pqxx::connection> connection_;
};

#endif //DATABASE_H

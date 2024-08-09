#include <server/database.h>

Database::Database() :
connection_data("dbname=postgres user=postgres password=root hostaddr=127.0.0.1 port=5432") {}

void Database::load_tables() {
    const std::string create_users_query = R"(
        CREATE TABLE IF NOT EXISTS users (
             id SERIAL PRIMARY KEY,
             name VARCHAR(15) NOT NULL
        );
    )";
    const std::string create_tasks_query = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id SERIAL PRIMARY KEY,
            title VARCHAR(100) NOT NULL,
            description TEXT,
            deadline_time TIMESTAMP,
            creation_time TIMESTAMP NOT NULL,
            creator_id INTEGER REFERENCES users(id),
            is_completed BOOLEAN NOT NULL DEFAULT FALSE
        );
    )";
    const std::string create_user_tasks_query = R"(
        CREATE TABLE IF NOT EXISTS user_tasks (
            user_id INTEGER REFERENCES users(id),
            task_id INTEGER REFERENCES tasks(id),
            role VARCHAR(20) NOT NULL,
            PRIMARY KEY(user_id, task_id)
        );
    )";
    try {
        pqxx::work w(*connection_);
        w.exec(create_users_query);
        w.exec(create_tasks_query);
        w.exec(create_user_tasks_query);
        w.commit();
        std::cout << "Table has been loaded" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error creating table: " << e.what() << std::endl;
        throw;
    }
}

void Database::add_user(const std::string& username) {
    std::string query = "INSERT INTO users (name) VALUES (" +
        connection_->quote(username)
    + ");";
    try {
        pqxx::work w(*connection_);
        w.exec(query);
        w.commit();
    } catch (const std::exception &e) {
        std::cerr << "Error adding user: " << e.what() << std::endl;
        throw;
    }
}

void Database::add_task(const Task& task, const std::set<int>& collaborators_id) {
    std::string creation_time = boost::posix_time::to_simple_string(task.get_creation_time());
    std::string dueDateStr = task.get_deadline() ? boost::posix_time::to_simple_string(task.get_deadline().value()) : "NULL";

    std::string query = "INSERT INTO tasks (title, description, deadline_time, creation_time, creator_id) VALUES (" +
        connection_->quote(task.get_title()) + ", " +
              (task.get_description() ? connection_->quote(task.get_description().value()) : "NULL") + ", " +
              (task.get_deadline() ? connection_->quote(dueDateStr) : "NULL") + ", " +
              connection_->quote(creation_time) + ", " +
              std::to_string(task.get_creator_id()) +
        ") RETURNING id;";
    try {
        pqxx::work w(*connection_);
        pqxx::result result = w.exec(query);
        int task_id = result[0]["id"].as<int>();
        w.commit();
        assign_users_to_task(task_id, task.get_creator_id(), collaborators_id);
    } catch (const std::exception &e) {
        std::cerr << "Error adding task: " << e.what() << std::endl;
        throw;
    }
}

void Database::assign_users_to_task(int task_id, int creator_id, const std::set<int> &collaborators_id) {
    const std::string assign_creator_query = "INSERT INTO user_tasks (user_id, task_id, role) VALUES (" +
                                           std::to_string(creator_id) + ", " +
                                           std::to_string(task_id) + ", 'creator');";
    try {
        pqxx::work w(*connection_);
        w.exec(assign_creator_query);

        for (const auto& collaborator_id : collaborators_id) {
            const std::string assign_collaborator_query = "INSERT INTO user_tasks (user_id, task_id, role) VALUES (" +
                                                    std::to_string(collaborator_id) + ", " +
                                                    std::to_string(task_id) + ", 'collaborator');";
            w.exec(assign_collaborator_query);
        }

        w.commit();
        std::cout << "Users assigned to task ID: " << task_id << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error assigning users to task: " << e.what() << std::endl;
        throw;
    }
}

std::vector<Task> Database::get_tasks_for_user(int user_id) {
    std::string query = R"(
        SELECT * FROM tasks
        JOIN user_tasks ON tasks.id = user_tasks.task_id
        WHERE user_tasks.user_id =
    )" + std::to_string(user_id) + ";";

    std::vector<Task> tasks;

    try {
        pqxx::work w(*connection_);
        const pqxx::result result = w.exec(query);

        for (const auto& row : result) {
            const int task_id = row["id"].as<int>();
            const auto title = row["title"].as<std::string>();
            const std::optional<std::string> description = row["description"].is_null() ?
                std::nullopt : std::optional(row["description"].as<std::string>());
            const std::optional<boost::posix_time::ptime> deadline = row["deadline_time"].is_null() ?
                std::nullopt : std::optional(boost::posix_time::time_from_string(row["deadline_time"].as<std::string>()));
            const boost::posix_time::ptime creation_time = boost::posix_time::time_from_string(row["creation_time"].as<std::string>());
            const int creator_id = row["creator_id"].as<int>();
            const bool is_completed = row["is_completed"].as<bool>();
            tasks.emplace_back(task_id, title, description, deadline, creation_time, creator_id, is_completed);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving tasks by user: " << e.what() << std::endl;
        throw;
    }

    return tasks;
}

Task Database::get_task_by_id(int task_id) {
    std::string query = "SELECT * FROM tasks WHERE id = " + std::to_string(task_id) + ";";

    try {
        pqxx::work w(*connection_);
        const pqxx::result result = w.exec(query);
        const pqxx::row row = result[0];

        const int id = row["id"].as<int>();
        const auto title = row["title"].as<std::string>();
        const std::optional<std::string> description = row["description"].is_null() ?
                    std::nullopt : std::optional(row["description"].as<std::string>());
        const std::optional<boost::posix_time::ptime> deadline = row["deadline_time"].is_null() ?
            std::nullopt : std::optional(boost::posix_time::time_from_string(row["deadline_time"].as<std::string>()));
        const boost::posix_time::ptime creation_time = boost::posix_time::time_from_string(row["creation_time"].as<std::string>());
        const int creator_id = row["creator_id"].as<int>();
        const bool is_completed = row["is_completed"].as<bool>();

        w.commit();
        return Task{id, title, description, deadline, creation_time, creator_id, is_completed};
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving tasks by id: " << e.what() << std::endl;
        throw;
    }
}

std::vector<User> Database::get_collaborators_for_task(int task_id) {
    const std::string query = R"(
        SELECT * FROM users
        JOIN user_tasks ON users.id = user_tasks.user_id
        WHERE user_tasks.task_id =
    )" + std::to_string(task_id) + R"( AND user_tasks.role = 'collaborator';)";

    std::vector<User> collaborators;

    try {
        pqxx::work w(*connection_);
        const pqxx::result result = w.exec(query);

        for (const auto& row : result) {
            const int user_id = row["id"].as<int>();
            const auto name = row["name"].as<std::string>();
            collaborators.emplace_back(user_id, name);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error retrieving collaborators for task ID " << task_id << ": " << e.what() << std::endl;
        throw;
    }

    return collaborators;
}

User Database::get_user_by_name(const std::string &username) {
    std::string query = "SELECT * FROM users WHERE name = " + connection_->quote(username) + ";";
    try {
        pqxx::work w(*connection_);
        const pqxx::result result = w.exec(query);

        const int id = result[0]["id"].as<int>();
        const auto name = result[0]["name"].as<std::string>();

        w.commit();
        return User{id, name};
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving user by name " << username << ": " << e.what() << std::endl;
        throw;
    }
}

User Database::get_user_by_id(int user_id) {
    const std::string query = "SELECT * FROM users WHERE id = " + std::to_string(user_id) + ";";

    try {
        pqxx::work w(*connection_);
        const pqxx::result result = w.exec(query);

        const int id = result[0]["id"].as<int>();
        const auto name = result[0]["name"].as<std::string>();

        w.commit();
        return User{id, name};
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving user by id " << user_id << ": " << e.what() << std::endl;
        throw;
    }
}

bool Database::user_exists(const std::string &username) {
    const std::string query = "SELECT 1 FROM users WHERE name = " +
        connection_->quote(username) + " LIMIT 1;";
    try {
        pqxx::work w(*connection_);
        const pqxx::result result = w.exec(query);
        return !result.empty();
    } catch (const std::exception &e) {
        std::cerr << "Error checking if user exists: " << e.what() << std::endl;
        return false;
    }
}

void Database::connect() {
    try {
        connection_ = std::make_unique<pqxx::connection>(connection_data);
        if (connection_->is_open()) {
            std::cout << "Connected to database: " << connection_->dbname() << std::endl;
        } else {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

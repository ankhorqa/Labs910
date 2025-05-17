#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

class User {
protected:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel) {
        if (name.empty()) throw std::invalid_argument("Некорректное имя пользователя.");
        if (accessLevel < 0) throw std::invalid_argument("Некорректный уровень доступа.");
        this->name = name;
        this->id = id;
        this->accessLevel = accessLevel;
    }

    virtual ~User() = default;

    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    virtual void displayInfo() const {
        std::cout << "Имя: " << name << ", ID: " << id << ", Уровень доступа: " << accessLevel << "\n";
    }
};

class Student : public User {
    std::string group;

public:
    Student(const std::string& name, int id, int accessLevel, const std::string& group)
        : User(name, id, accessLevel), group(group) {}

    void displayInfo() const override {
        std::cout << "Студент: " << name << ", Группа: " << group << ", Доступ: " << accessLevel << "\n";
    }
};

class Teacher : public User {
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
        : User(name, id, accessLevel), department(department) {}

    void displayInfo() const override {
        std::cout << "Преподаватель: " << name << ", Кафедра: " << department << ", Доступ: " << accessLevel << "\n";
    }
};

class Administrator : public User {
    std::string role;

public:
    Administrator(const std::string& name, int id, int accessLevel, const std::string& role)
        : User(name, id, accessLevel), role(role) {}

    void displayInfo() const override {
        std::cout << "Администратор: " << name << ", Роль: " << role << ", Доступ: " << accessLevel << "\n";
    }
};


class Resource {
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int level) {
        if (name.empty() || level < 0)
            throw std::invalid_argument("Неверные данные ресурса");

        this->name = name;
        this->requiredAccessLevel = level;
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayInfo() const {
        std::cout << "Ресурс: " << name << ", Требуемый уровень доступа: " << requiredAccessLevel << "\n";
    }
};

template<typename TUser, typename TResource>
class AccessControlSystem {
    std::vector<std::shared_ptr<TUser>> users;
    std::vector<std::shared_ptr<TResource>> resources;

public:
    void addUser(std::shared_ptr<TUser> user) {
        users.push_back(user);
    }

    void addResource(std::shared_ptr<TResource> resource) {
        resources.push_back(resource);
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
            std::cout << "---\n";
        }
    }

    void checkAccessForAll() const {
        for (const auto& user : users) {
            for (const auto& res : resources) {
                std::cout << user->getName() << (res->checkAccess(*user) ? " имеет доступ к " : " НЕ имеет доступ к ");
                res->displayInfo();
            }
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        for (const auto& user : users) {
            file << user->getName() << "," << user->getId() << "," << user->getAccessLevel() << "\n";
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            int id, level;
            char comma;
            if (iss >> name >> comma >> id >> comma >> level) {
                addUser(std::make_shared<User>(name, id, level));
            }
        }
    }

    std::shared_ptr<TUser> findUserByName(const std::string& name) const {
        for (const auto& user : users) {
            if (user->getName() == name)
                return user;
        }
        return nullptr;
    }

    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(), [](const std::shared_ptr<TUser>& a, const std::shared_ptr<TUser>& b) {
            return a->getAccessLevel() > b->getAccessLevel();
        });
    }
};

int main() {
    try {
        AccessControlSystem<User, Resource> system;

        system.addUser(std::make_shared<Student>("Игорь", 1, 2, "02"));
        system.addUser(std::make_shared<Administrator>("Виктор", 3, 5, "Системный администратор"));
        system.addUser(std::make_shared<Teacher>("Наталья", 2, 3, "Информатика"));

        system.addResource(std::make_shared<Resource>("Библиотека", 1));
        system.addResource(std::make_shared<Resource>("Лаборатория", 3));
        system.addResource(std::make_shared<Resource>("Серверная комната", 5));

        std::cout << "--- Все пользователи ---\n";
        system.displayAllUsers();

        std::cout << "--- Проверка доступа ---\n";
        system.checkAccessForAll();

        std::cout << "--- Сортировка пользователей по уровню доступа ---\n";
        system.sortUsersByAccessLevel();
        system.displayAllUsers();

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}

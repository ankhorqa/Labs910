#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>


template<typename T>
class Logger {
public:
    static void log(const T& message) {
        std::ofstream file("game_log.txt", std::ios::app);
        file << message << std::endl;
    }
};


class Inventory {
private:
    std::vector<std::string> items;
public:
    void addItem(const std::string& item) {
        items.push_back(item);
    }
    void showItems() const {
        std::cout << "Инвентарь: ";
        for (const auto& item : items) std::cout << item << ", ";
        std::cout << std::endl;
    }
    const std::vector<std::string>& getItems() const { return items; }
};

class Character {
private:
    std::string name;
    int health, attack, defense, level, experience;
    Inventory inventory;

public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d), level(1), experience(0) {}

    void takeDamage(int dmg) {
        health -= dmg;
        if (health <= 0) {
            throw std::runtime_error(name + " погиб!");
        }
    }
    
    void attackEnemy(class Enemy& monster);
    
    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            std::cout << name << " повысил уровень до " << level << "!" << std::endl;
            Logger<std::string>::log(name + " повысил уровень до " + std::to_string(level));
        }
    }
    
    void heal(int amount) {
        health += amount;
        if (health > 100) {
            health = 100;
        }
        std::cout << name << " восстановил " << amount << " HP!" << std::endl;
        Logger<std::string>::log(name + " восстановил " + std::to_string(amount) + " HP");
    }


    void displayInfo() const {
        std::cout << "Имя: " << name << ", HP: " << health
                  << ", Атака: " << attack << ", Защита: " << defense
                  << ", Уровень: " << level << ", Опыт: " << experience << std::endl;
    }

    void showInventory() const {
        inventory.showItems(); 
        
    }

    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    Inventory& getInventory() { return inventory; }
};


class Enemy {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    virtual ~Enemy() {}
    virtual void displayInfo() const {
        std::cout << "Монстр: " << name << ", HP: " << health << std::endl;
    }
    virtual void takeDamage(int dmg) {
        health -= dmg;
        if (health <= 0) throw std::runtime_error(name + " убит!");
    }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    const std::string& getName() const { return name; }
};

class Goblin : public Enemy {
public:
    Goblin() { name = "Гоблин"; health = 25; attack = 8; defense = 5; }
};


class Skeleton : public Enemy {
public:
    Skeleton() { name = "Скелет"; health = 50; attack = 15; defense = 10; }
};

class Dragon : public Enemy {
public:
    Dragon() { name = "Дракон"; health = 100; attack = 25; defense = 15; }
};

void Character::attackEnemy(Enemy& monster) {
    int damage = attack - monster.getDefense();
    if (damage > 0) {
        std::cout << name << " атакует " << monster.getName() << " и наносит " << damage << " урона!" << std::endl;
        Logger<std::string>::log(name + " атакует " + monster.getName() + " и наносит " + std::to_string(damage) + " урона!");
        monster.takeDamage(damage);
    } else {
        std::cout << name << " атакует " << monster.getName() << " без урона!" << std::endl;
    }
}

class Game {
private:
    Character player;

public:
    Game(const std::string& playerName) : player(playerName, 100, 20, 5) {}

    void start() {
        player.displayInfo();
    }

    void battle(Enemy& monster) {
        try {
            while (true) {
                player.attackEnemy(monster);
                player.displayInfo();
                monster.displayInfo();
                player.takeDamage(monster.getAttack() - player.getDefense());
            }
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            Logger<std::string>::log(e.what());
        }
    }

    void saveGame() {
        std::ofstream file("save.txt");
        if (!file) throw std::runtime_error("Невозможно открыть файл для сохранения!");
        file << player.getName() << "\n" << player.getHealth() << std::endl;
        for (const auto& item : player.getInventory().getItems()) file << item << " ";
        file << std::endl;
    }

    void loadGame() {
        std::ifstream file("save.txt");
        if (!file) throw std::runtime_error("Невозможно открыть файл для загрузки!");
        std::string name;
        int hp;
        file >> name >> hp;
        player = Character(name, hp, 20, 5);
        std::string item;
    }
};

int main() {
    Game game("Герой");
    game.start();
    Skeleton skelet;
    game.battle(skelet);
    game.saveGame();
    return 0;
}

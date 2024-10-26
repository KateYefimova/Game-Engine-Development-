#include <iostream>
#include <vector>
#include <memory>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <locale>
class Coordinate {
private:
    int x, y;

public:
    Coordinate(int x = 0, int y = 0) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }

    void set(int newX, int newY) { x = newX; y = newY; }

    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
};

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

class Snake {
private:
    Coordinate head;
    std::vector<Coordinate> tail;

public:
    Direction dir;

    Snake(int startX, int startY) : head(startX, startY), dir(STOP) {}

    Coordinate getHead() const { return head; }
    const std::vector<Coordinate>& getTail() const { return tail; }

    void Move() {
        Coordinate prev = head;
        Coordinate prev2;
        for (int i = 0; i < tail.size(); ++i) {
            prev2 = tail[i];
            tail[i] = prev;
            prev = prev2;
        }

        switch (dir) {
        case LEFT: head.set(head.getX() - 1, head.getY()); break;
        case RIGHT: head.set(head.getX() + 1, head.getY()); break;
        case UP: head.set(head.getX(), head.getY() - 1); break;
        case DOWN: head.set(head.getX(), head.getY() + 1); break;
        default: break;
        }
    }

    void Grow() {
        tail.push_back(Coordinate(head.getX(), head.getY()));
    }

    bool CollidesWithSelf() const {
        for (const auto& segment : tail) {
            if (segment == head) return true;
        }
        return false;
    }
};

class Product {
protected:
    Coordinate position;
public:
    Product() {}
    virtual int getScore() const = 0;
    virtual char getSymbol() const = 0;
    Coordinate getPosition() const { return position; }
    virtual void PlaceProduct(int width, int height, const std::vector<Coordinate>& snakeTail, const std::vector<Coordinate>& walls) = 0;
    virtual void Reset(int width, int height, const std::vector<Coordinate>& snakeTail, const std::vector<Coordinate>& walls) = 0;
};

class Apple : public Product {
public:
    Apple(int maxX, int maxY) {
        PlaceProduct(maxX, maxY, {}, {}); // Call PlaceProduct during initialization
    }
    int getScore() const override { return 10; }

    void PlaceProduct(int maxX, int maxY, const std::vector<Coordinate>& snakeTail, const std::vector<Coordinate>& walls) override {
        int x, y;
        do {
            x = rand() % maxX;
            y = rand() % maxY;
        } while (isOnSnake(x, y, snakeTail) || isOnWall(x, y, walls));

        position.set(x, y);
    }

    bool isOnSnake(int appleX, int appleY, const std::vector<Coordinate>& snakeTail) {
        for (const auto& segment : snakeTail) {
            if (segment.getX() == appleX && segment.getY() == appleY) return true;
        }
        return false;
    }
    bool isOnWall(int appleX, int appleY, const std::vector<Coordinate>& walls) {
        for (const auto& wall : walls) {
            if (wall.getX() == appleX && wall.getY() == appleY) return true;
        }
        return false;
    }
    char getSymbol() const override { return 'A'; }

    void Reset(int maxX, int maxY, const std::vector<Coordinate>& snakeTail, const std::vector<Coordinate>& walls) override{
        PlaceProduct(maxX, maxY, snakeTail, walls); // Оновлення позиції продукту
    }
};

class Banana : public Product {
public:
    Banana(int maxX, int maxY) {
        PlaceProduct(maxX, maxY, {}, {}); // Call PlaceProduct during initialization
    }
    int getScore() const override { return 25; }

    void PlaceProduct(int maxX, int maxY, const std::vector<Coordinate>& snakeTail, const std::vector<Coordinate>& walls) override {
        int x, y;
        do {
            x = rand() % maxX;
            y = rand() % maxY;
        } while (isOnSnake(x, y, snakeTail) || isOnWall(x, y, walls));

        position.set(x, y);
    }

    bool isOnSnake(int bananaX, int bananaY, const std::vector<Coordinate>& snakeTail) {
        for (const auto& segment : snakeTail) {
            if (segment.getX() == bananaX && segment.getY() == bananaY) return true;
        }
        return false;
    }
    bool isOnWall(int bananaX, int bananaY, const std::vector<Coordinate>& walls) {
        for (const auto& wall : walls) {
            if (wall.getX() == bananaX && wall.getY() == bananaY) return true;
        }
        return false;
    }

    char getSymbol() const override { return 'B'; }
    void Reset(int maxX, int maxY, const std::vector<Coordinate>& snakeTail, const std::vector<Coordinate>& walls) override {
        PlaceProduct(maxX, maxY, snakeTail, walls); // Оновлення позиції продукту
    }
};

class Player {
public:
    void controlSnake(Snake& snake) {
        if (_kbhit()) {
            switch (_getch()) {
            case 'a':
                if (snake.dir != RIGHT) snake.dir = LEFT;
                break;
            case 'd':
                if (snake.dir != LEFT) snake.dir = RIGHT;
                break;
            case 'w':
                if (snake.dir != DOWN) snake.dir = UP;
                break;
            case 's':
                if (snake.dir != UP) snake.dir = DOWN;
                break;
            }
        }
    }
};

class Map {
private:
    int width, height;
    std::vector<Coordinate> walls;
    int maxRandomWalls;
    

    void InitializeRandomWalls() {
        walls.clear();
        int wallCount = 0;
        while (wallCount < maxRandomWalls) {
            int x = rand() % (width - 2) + 1;   // Генеруємо координати, щоб не заходили на межі
            int y = rand() % (height - 2) + 1;

            Coordinate newWall(x, y);

            // Перевіряємо, чи немає вже стіни на цьому місці
            if (std::find(walls.begin(), walls.end(), newWall) == walls.end()) {
                walls.push_back(newWall);
                wallCount++;
            }
        }
    }

public:
    Map(int w, int h, int maxWalls) : width(w), height(h), maxRandomWalls(maxWalls) {
       
        InitializeRandomWalls();
    }

    bool CheckCollisionWithWalls(const Coordinate& coord) const {
        return std::find(walls.begin(), walls.end(), coord) != walls.end();
    }

    const std::vector<Coordinate>& getWalls() const {
        return walls;
    }
    void ResetWalls() {
        InitializeRandomWalls(); // Перегенерація стін
    }
};

class Renderer {
public:
    void Draw(int width, int height, const Snake& snake, const std::vector<std::unique_ptr<Product>>& products, int score, const Map& map) const {
        system("cls");
        for (int i = 0; i < width + 2; ++i) std::cout << "_";
        std::cout << std::endl;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == 0) std::cout << "|";

                if (x == snake.getHead().getX() && y == snake.getHead().getY()) {
                    std::cout << "O";
                }
                else {
                    bool printed = false;

                    for (const auto& wall : map.getWalls()) {
                        if (x == wall.getX() && y == wall.getY()) {
                            std::cout << "#";
                            printed = true;
                            break;
                        }
                    }

                    if (!printed) {
                        for (const auto& product : products) {
                            if (x == product->getPosition().getX() && y == product->getPosition().getY()) {
                                std::cout << product->getSymbol();
                                printed = true;
                                break;
                            }
                        }
                    }

                    if (!printed) {
                        for (const auto& t : snake.getTail()) {
                            if (t.getX() == x && t.getY() == y) {
                                std::cout << "o";
                                printed = true;
                                break;
                            }
                        }
                    }

                    if (!printed) std::cout << " ";
                }
                if (x == width - 1) std::cout << "|";
            }
            std::cout << std::endl;
        }

        for (int i = 0; i < width + 2; ++i) std::cout << "_";
        std::cout << "\nScore: " << score << std::endl;
    }
};

class Game {
private:
    int width, height, score;
    bool gameOver;
    Snake snake;
    std::vector<std::unique_ptr<Product>> products;
    Player player;
    Renderer renderer;
    const int maxSnakeSize;
    Map map;
    const int WALL_UPDATE_INTERVAL = 10 * 1000; 
    DWORD lastWallUpdateTime;
    std::vector<int> scores;
public:
    Game(int w, int h)
        : width(w), height(h), score(0), gameOver(false), snake(w / 2, h / 2), map(w, h, 5), maxSnakeSize(w* h - 1), lastWallUpdateTime(GetTickCount64()) {
        products.emplace_back(std::make_unique<Apple>(w, h));
        products.emplace_back(std::make_unique<Banana>(w, h));
    }
    void ResetGame() {
        score = 0;
        gameOver = false;
        snake = Snake(width / 2, height / 2); 
        map.ResetWalls(); 
        for (auto& product : products) {
            product->Reset(width, height, snake.getTail(), map.getWalls()); 
        }
    }

    void Run() {
        ResetGame();
        renderer.Draw(width, height, snake, products, score, map);
        while (!gameOver) {
            player.controlSnake(snake);
            DWORD currentTime = GetTickCount64();
           
            if (snake.dir != STOP) {
                snake.Move();

                if (snake.CollidesWithSelf() || map.CheckCollisionWithWalls(snake.getHead()) ||
                    (snake.getHead().getX() < 0 || snake.getHead().getX() >= width || snake.getHead().getY() < 0 || snake.getHead().getY() >= height)) {
                    gameOver = true;
                    std::cout << "Game Over!" << std::endl;
                    SaveScore(score);
                    return;
                }

                for (auto& product : products) {
                    if (snake.getHead() == product->getPosition()) {
                        score += product->getScore(); 
                        snake.Grow();
                        product->Reset(width, height, snake.getTail(), map.getWalls()); 
                    }
                }

                renderer.Draw(width, height, snake, products, score, map);
            }
            if (currentTime - lastWallUpdateTime >= WALL_UPDATE_INTERVAL) {
                map.ResetWalls();
                lastWallUpdateTime = currentTime;
            }

            Sleep(300);
        }
        ResetGame();

    }
    void SaveScore(int newScore) {
        scores.push_back(newScore);
        if (scores.size() > 5) {
            scores.erase(scores.begin()); 
        }
    }

    const std::vector<int>& GetScores() const {
        return scores; 
    }
};

class Menu {
private:
    Game game; 

public:
    Menu() : game(10, 10) {} // Конструктор, ініціалізує гру

    void ShowMainMenu() {
        

        int choice;
        while (true) { 
            system("CLS"); 
            std::cout << "===== Main Menu =====" << std::endl;
            std::cout << "1. Start game" << std::endl;
            std::cout << "2. Show result of the last 5 games" << std::endl;
            std::cout << "3. Exit" << std::endl;
            std::cout << "Enter the number: ";
            std::cin >> choice;

            switch (choice) {
            case 1:
                StartGame();
                break;
            case 2:
                ShowScores();
                break;
            case 3:
                exit(0); 
            default:
                std::cout << "Incorrect input." << std::endl;
            }
        }
       

    }

    void StartGame() {
        system("CLS"); 

        srand(static_cast<unsigned int>(time(0))); 
        game.Run(); 
        std::cout << "Enter exit to return to the main menu.." << std::endl;
        std::cin.ignore(); 
        std::cin.get();
    }

    void ShowScores() {
        system("CLS"); 

        std::cout << "===== Results =====" << std::endl;
        const std::vector<int>& scores = game.GetScores(); 
        if (scores.empty()) {
            std::cout << "There is no results." << std::endl;
        }
        else {
            for (size_t i = 0; i < scores.size(); ++i) {
                std::cout << "Game " << i + 1 << ": " << scores[i]  << std::endl;
            }
        }
        std::cout << "Enter exit to return to the main menu." << std::endl;
        std::cin.ignore(); 
        std::cin.get(); 
    }
};


int main() {
    Menu menu;
    menu.ShowMainMenu();
    return 0;
}

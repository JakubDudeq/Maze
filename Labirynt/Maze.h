#pragma once
#ifndef MAZE_H
#define MAZE_H

class Maze {
private:
    enum Dir { UP, DOWN, RIGHT, LEFT, CROSSINGUP, CROSSINGDOWN, CROSSINGRIGHT, CROSSINGLEFT };

    char player = 'P';
    char trail = 'T';
    char fakeWall = 'F';
    Dir direction = Dir::UP;
    int playerY;
    int playerX;
    bool found = false;
    int maxFailCount = 0;
    int failCount = 0;
    List<Dir> path;
    string structure;
    int rows = 0;
    int columns = 0;
    char** matrix;
    int startPosition[2] = { -1, -1 };
    int endPosition[2] = { -1, -1 };

    bool isInDeadEnd();
    bool isInDeadEndIgnoreTrail();
    bool isCrossing(Dir d);
    Dir getOpposition(Dir d);
    void goOppositeWay(Dir d);
    Dir getDirectionFromCrossing(Dir d);
    void goWithDirection(Dir d);
    int lookAround();
    int lookAroundIgnoreDirection();
    void saveCrossings();
    void saveCrossingsIgnoreDirections();
    void chooseCrossing(Dir d);
    void createFakeWall(Dir d);
    void explore();
    void go(Dir r);
    void goUp(bool exploring);
    void goDown(bool exploring);
    void goLeft(bool exploring);
    void goRight(bool exploring);
    bool canGo(Dir d);
    bool canGoIgnoreTrail(Dir d);
    bool canUp();
    bool canDown();
    bool canLeft();
    bool canRight();
    bool canUpIgnoreTrail();
    bool canDownIgnoreTrail();
    bool canLeftIgnoreTrail();
    bool canRightIgnoreTrail();
    void rotate();
    bool isValidCharacter(char a);
    void positionPlayerAtStart();
    void isItEnd();
    string changeValueToDirection(Dir x);
    void showRoute();

public:
    enum Colors { BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, MAGENTA = 5, BROWN = 6, LIGHTGRAY = 7, DARKGRAY = 8, LIGHTBLUE = 9, LIGHTGREEN = 10, LIGHTCYAN = 11, LIGHTRED = 12, LIGHTMAGENTA = 13, YELLOW = 14, WHITE = 15 };
    enum Size { SMALL = 1, BIG = 2 };

    int wallColor = Colors::BROWN;
    int airColor = Colors::WHITE;
    int startColor = Colors::RED;
    int endColor = Colors::GREEN;
    int playerColor = Colors::BLUE;
    int borderColor = Colors::DARKGRAY;
    int pathColor = Colors::CYAN;
    int fakeWallColor = Colors::LIGHTGRAY;
    char wall = '1';
    char air = '0';
    char start = 'S';
    char end = 'E';
    double speed = 0;
    int size = Size::BIG;

    Maze(const string& filename);
    void printMatrix();
    void run();
    void render();
    void renderAndRun();
};

#endif // MAZE_H

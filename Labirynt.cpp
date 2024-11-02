#include "dudqLIST.h"

#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>

using namespace std;


void wait(double seconds) {
	this_thread::sleep_for(chrono::milliseconds(static_cast<int>(seconds * 1000)));
}

void setConsoleColor(int textColor, int bgColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

class Maze {
private:
	enum Dir {
		UP, DOWN, RIGHT, LEFT, CROSSINGUP, CROSSINGDOWN, CROSSINGRIGHT, CROSSINGLEFT
	};

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

	bool isInDeadEnd() {
		bool check = true;
		if (direction != Dir::UP && canDown()) { check = false; }
		if (direction != Dir::DOWN && canUp()) { check = false; }
		if (direction != Dir::RIGHT && canLeft()) { check = false; }
		if (direction != Dir::LEFT && canRight()) { check = false; }
		return check;
	}
	bool isInDeadEndIgnoreTrail() {
		bool check = true;
		if (direction != Dir::UP && canDownIgnoreTrail()) { check = false; }
		if (direction != Dir::DOWN && canUpIgnoreTrail()) { check = false; }
		if (direction != Dir::RIGHT && canLeftIgnoreTrail()) { check = false; }
		if (direction != Dir::LEFT && canRightIgnoreTrail()) { check = false; }
		return check;
	}
	bool isCrossing(Dir d) {
		if (d == Dir::CROSSINGDOWN || d == Dir::CROSSINGUP || d == Dir::CROSSINGLEFT || d == Dir::CROSSINGRIGHT) {
			return true;
		}
		return false;
	}
	Dir  getOpposition(Dir d) {
		if (d == Dir::DOWN) { return Dir::UP; }
		if (d == Dir::UP) { return Dir::DOWN; }
		if (d == Dir::LEFT) { return Dir::RIGHT; }
		if (d == Dir::RIGHT) { return Dir::LEFT; }
	}
	void goOppositeWay(Dir d) {
		if (d == Dir::UP) { goDown(false); }
		else if (d == Dir::DOWN) { goUp(false); }
		else if (d == Dir::RIGHT) { goLeft(false); }
		else if (d == Dir::LEFT) { goRight(false); }
	}
	Dir getDirectionFromCrossing(Dir d) {
		if (d == Dir::CROSSINGUP) { return Dir::UP; }
		else if (d == Dir::CROSSINGDOWN) { return Dir::DOWN; }
		else if (d == Dir::CROSSINGRIGHT) { return Dir::RIGHT; }
		else if (d == Dir::CROSSINGLEFT) { return Dir::LEFT; }
	}
	void goWithDirection(Dir d) {
		if (d == Dir::UP) { goUp(true); }
		else if (d == Dir::DOWN) { goDown(true); }
		else if (d == Dir::RIGHT) { goRight(true); }
		else if (d == Dir::LEFT) { goLeft(true); }
	}
	int  lookAround() {

		short count = 0;

		if (canUp() && direction != Dir::DOWN) { count++; }
		if (canDown() && direction != Dir::UP) { count++; }
		if (canLeft() && direction != Dir::RIGHT) { count++; }
		if (canRight() && direction != Dir::LEFT) { count++; }

		return count;
	}
	int  lookAroundIgnoreDirection() {

		short count = 0;

		if (canUp()) { count++; }
		if (canDown()) { count++; }
		if (canLeft()) { count++; }
		if (canRight()) { count++; }

		return count;
	}
	void saveCrossings() {
		if (canUp() && direction != Dir::DOWN) { path.append(Dir::CROSSINGUP); }
		if (canDown() && direction != Dir::UP) { path.append(Dir::CROSSINGDOWN); }
		if (canLeft() && direction != Dir::RIGHT) { path.append(Dir::CROSSINGLEFT); }
		if (canRight() && direction != Dir::LEFT) { path.append(Dir::CROSSINGRIGHT); }
	}
	void saveCrossingsIgnoreDirections() {
		if (canUp()) { path.append(Dir::CROSSINGUP); }
		if (canDown()) { path.append(Dir::CROSSINGDOWN); }
		if (canLeft()) { path.append(Dir::CROSSINGLEFT); }
		if (canRight()) { path.append(Dir::CROSSINGRIGHT); }
	}
	void chooseCrossing(Dir d) {
		if (d == Dir::CROSSINGUP) { goUp(true); }
		else if (d == Dir::CROSSINGDOWN) { goDown(true); }
		else if (d == Dir::CROSSINGRIGHT) { goRight(true); }
		else if (d == Dir::CROSSINGLEFT) { goLeft(true); }
	}
	void createFakeWall(Dir d) {
		if (d == Dir::UP) {
			matrix[playerY + 1][playerX] = fakeWall;
		}
		else if (d == Dir::DOWN) {
			matrix[playerY - 1][playerX] = fakeWall;
		}
		else if (d == Dir::RIGHT) {
			matrix[playerY ][playerX - 1] = fakeWall;
		}
		else if (d == Dir::LEFT) {
			matrix[playerY][playerX + 1] = fakeWall;
		}
		
	}
	void explore() {
		if (path.length == 0 || failCount > maxFailCount || found) {
			found = false;
		}
		else if (endPosition[0] == playerY && endPosition[1] == playerX) {
			found = true;
		}
		else if (isInDeadEnd() && !isCrossing(path.last())) {
			while (path.length != 0 && !isCrossing(path.last())) {
				goOppositeWay(path.last());
				if (path.length >= 2 && isCrossing(path.of(path.length - 2))) {
					createFakeWall(direction);
				}
				path.deleteIndex(path.length - 1);
			}
		}
		else if (path.length != 0 && isCrossing(path.last())) {
			int crossingIndex = path.length - 1;
			if (canGoIgnoreTrail(getDirectionFromCrossing(path.of(crossingIndex)))) {
				chooseCrossing(path.last());
			}
			path.deleteIndex(crossingIndex);
		}
		else {
			if (canGo(direction) && !isInDeadEnd()) {
				if (lookAround() == 1) {
					go(direction);
				}
				else if (lookAround() > 1) {
					saveCrossings();
				}
			}
			else if (!canGo(direction)) {
				if (lookAround() == 1) {
					rotate();
				}
				else if (lookAround() > 1) {
					saveCrossings();
					if (path.length != 0) {
						int crossingIndex = path.length - 1;
						if (canGoIgnoreTrail(getDirectionFromCrossing(path.of(crossingIndex)))) {
							chooseCrossing(path.last());
						}
						path.deleteIndex(crossingIndex);
					}
				}
			}
		}
	}


	void go(Dir r) {
		// D
			if (r == Dir::UP) {
				goUp(true);
				isItEnd();
			}
			else if (r == Dir::DOWN) {
				goDown(true);
				isItEnd();
			}
			else if (r == Dir::LEFT) {
				goLeft(true);
			}
			else if (r == Dir::RIGHT) {
				goRight(true);
			}
		}
	void goUp(bool exploring) {
			if (endPosition[0] == playerY && endPosition[1] == playerX || found == true) {
				// U
			}
			else {
				wait(speed);
				direction = Dir::UP;
				if (startPosition[0] == playerY && startPosition[1] == playerX) {
					matrix[playerY][playerX] = start;
					failCount++;
				}
				else {
					if (!exploring) {
						matrix[playerY][playerX] = air;
					}
					else {
						matrix[playerY][playerX] = trail;
					}
				}
				if (exploring) { path.append(Dir::UP); }
				playerY -= 1;
				matrix[playerY][playerX] = player;
				render();
			}
		}
	void goDown(bool exploring) {
			if (endPosition[0] == playerY && endPosition[1] == playerX || found == true) {
				// D
			}
			else {
				wait(speed);
				direction = Dir::DOWN;
				if (startPosition[0] == playerY && startPosition[1] == playerX) {
					matrix[playerY][playerX] = start;
					failCount++;
				}
				else {
					if (!exploring) {
						matrix[playerY][playerX] = air;
					}
					else {
						matrix[playerY][playerX] = trail;
					}
				}
				if (exploring) { path.append(Dir::DOWN); }
				playerY += 1;
				matrix[playerY][playerX] = player;
				render();
			}
		}
	void goLeft(bool exploring) {
			if (endPosition[0] == playerY && endPosition[1] == playerX || found == true) {
				// E
			}
			else {
				wait(speed);
				direction = Dir::LEFT;
				if (startPosition[0] == playerY && startPosition[1] == playerX) {
					matrix[playerY][playerX] = start;
					failCount++;
				}
				else {
					if (!exploring) {
						matrix[playerY][playerX] = air;
					}
					else {
						matrix[playerY][playerX] = trail;
					}
				}
				if (exploring) { path.append(Dir::LEFT); }
				playerX -= 1;
				matrix[playerY][playerX] = player;
				render();
			}
		}
	void goRight(bool exploring) {
			if (endPosition[0] == playerY && endPosition[1] == playerX || found == true) {
				// K
			}
			else {
				wait(speed);
				direction = Dir::RIGHT;
				if (startPosition[0] == playerY && startPosition[1] == playerX) {
					matrix[playerY][playerX] = start;
					failCount++;
				}
				else {
					if (!exploring) {
						matrix[playerY][playerX] = air;
					}
					else {
						matrix[playerY][playerX] = trail;
					}
				}
				if (exploring) { path.append(Dir::RIGHT); }
				playerX += 1;
				matrix[playerY][playerX] = player;
				render();
			}
		}

	bool canGo(Dir d) {
			if (d == Dir::DOWN) {
				return canDown();
			}
			if (d == Dir::UP) {
				return canUp();
			}
			if (d == Dir::LEFT) {
				return canLeft();
			}
			if (d == Dir::RIGHT) {
				return canRight();
			}
		}
	bool canGoIgnoreTrail(Dir d) {
		if (d == Dir::DOWN) {
			return canDownIgnoreTrail();
		}
		if (d == Dir::UP) {
			return canUpIgnoreTrail();
		}
		if (d == Dir::LEFT) {
			return canLeftIgnoreTrail();
		}
		if (d == Dir::RIGHT) {
			return canRightIgnoreTrail();
		}
		return false;
	}
	bool canUp() {
			if (playerY == 0) {
				return false;
			}
			else if (matrix[playerY - 1][playerX] == wall || matrix[playerY - 1][playerX] == trail || matrix[playerY - 1][playerX] == start || matrix[playerY - 1][playerX] == fakeWall) {
				return false;
			}
			else {
				return true;
			}
		}
	bool canDown() {
			if (playerY == columns - 1) {
				return false;
			}
			else if (matrix[playerY + 1][playerX] == wall || matrix[playerY + 1][playerX] == trail || matrix[playerY + 1][playerX] == start || matrix[playerY + 1][playerX] == fakeWall) {
				return false;
			}
			else {
				return true;
			}
		}
	bool canLeft() {
			if (playerX == 0) {
				return false;
			}
			else if (matrix[playerY][playerX - 1] == wall || matrix[playerY][playerX - 1] == trail || matrix[playerY][playerX - 1] == start || matrix[playerY][playerX - 1] == fakeWall) {
				return false;
			}
			else {
				return true;
			}
		}
	bool canRight() {
			if (playerX == rows - 1) {
				return false;
			}
			else if (matrix[playerY][playerX + 1] == wall || matrix[playerY][playerX + 1] == trail || matrix[playerY][playerX + 1] == start || matrix[playerY][playerX + 1] == fakeWall) {
				return false;
			}
			else {
				return true;
			}
		}
	bool canUpIgnoreTrail() {
		if (playerY == 0) {
			return false;
		}
		else if (matrix[playerY - 1][playerX] == wall || matrix[playerY - 1][playerX] == fakeWall) {
			return false;
		}
		else {
			return true;
		}
	}
	bool canDownIgnoreTrail() {
		if (playerY == columns - 1) {
			return false;
		}
		else if (matrix[playerY + 1][playerX] == wall || matrix[playerY + 1][playerX] == fakeWall) {
			return false;
		}
		else {
			return true;
		}
	}
	bool canLeftIgnoreTrail() {
		if (playerX == 0) {
			return false;
		}
		else if (matrix[playerY][playerX - 1] == wall || matrix[playerY][playerX - 1] == fakeWall) {
			return false;
		}
		else {
			return true;
		}
	}
	bool canRightIgnoreTrail() {
		if (playerX == rows - 1) {
			return false;
		}
		else if (matrix[playerY][playerX + 1] == wall || matrix[playerY][playerX + 1] == fakeWall) {
			return false;
		}
		else {
			return true;
		}
	}


	void rotate() {
			if (direction == Dir::UP || direction == Dir::DOWN) {
				if (canLeft()) { direction = Dir::LEFT; }
				else if (canRight()) { direction = Dir::RIGHT; }
			}
			if (direction == Dir::LEFT || direction == Dir::RIGHT) {
				if (canUp()) { direction = Dir::UP; }
				else if (canDown()) { direction = Dir::DOWN; }
			}
		}

	bool isValidCharacter(char a) {
			if (a == wall || a == air || a == start || a == end || a == trail) {
				return true;
			}
			else {
				return false;
			}
		}
	void positionPlayerAtStart() {
			for (int i = 0; i < columns; i++) {
				for (int j = 0; j < rows; j++) {
					if (matrix[i][j] == start) {
						playerY = i;
						playerX = j;
						startPosition[0] = i;
						startPosition[1] = j;
						matrix[i][j] = 'P';
					}
					if (matrix[i][j] == end) {
						endPosition[0] = i;
						endPosition[1] = j;
					}
				}
			}
		}
	void isItEnd() {
		if (endPosition[0] == playerY && endPosition[1] == playerX || found == true) {
			found = true;
		}
	}

	string changeValueToDirection(Dir x) {
			if (x == Dir::UP) { return "UP"; }
			if (x == Dir::DOWN) { return "DOWN"; }
			if (x == Dir::RIGHT) { return "RIGHT"; }
			if (x == Dir::LEFT) { return "LEFT"; }
		}
	void showRoute() {
			for (int i = 0; i < path.length; i++) {
				cout << i + 1 << ". " << changeValueToDirection(path.of(i)) << endl;
			}
		}

public:
	enum Colors {
		BLACK = 0,
		BLUE = 1,
		GREEN = 2,
		CYAN = 3,
		RED = 4,
		MAGENTA = 5,
		BROWN = 6,
		LIGHTGRAY = 7,
		DARKGRAY = 8,
		LIGHTBLUE = 9,
		LIGHTGREEN = 10,
		LIGHTCYAN = 11,
		LIGHTRED = 12,
		LIGHTMAGENTA = 13,
		YELLOW = 14,
		WHITE = 15
	};
	enum Size {
		SMALL = 1,
		BIG = 2
	};

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

	Maze(const string& filename) {

		int thisRows = 0;
		int thisColumns = 0;

		ifstream file(filename);
		if (file.is_open()) {
			string line;
			while (getline(file, line)) {
				thisRows++;
				structure += line + '\n';
			}
			file.close();

			for (int i = 0; i < structure.length(); i++) {
				if (structure[i] != '\n' && isValidCharacter(structure[i])) {
					thisColumns++;
				}
				else if(structure[i] == '\n') {
					break;
				}
			}
			matrix = new char* [thisRows];
			for (int i = 0; i < thisRows; ++i) {
				matrix[i] = new char[thisColumns];
			}
			for (int i = 0; i < thisRows; ++i) {
				for (int j = 0; j < thisColumns; ++j) {
					matrix[i][j] = 0;
				}
			}

			thisRows = 0;
			thisColumns = 0;
			for (int i = 0; i < structure.length(); i++) {
				if (structure[i] == '\n') {
					if (thisRows > rows) {
						rows = thisRows;
					}
					thisRows = 0;
					thisColumns++;
				}
				else if (isValidCharacter(structure[i])){
					matrix[thisColumns][thisRows] = structure[i];
					thisRows++;
				}
			}
			columns = thisColumns;
		}
		positionPlayerAtStart();
	}
	
	void printMatrix() {
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < columns; ++j) {
				cout << "[" << matrix[i][j] <<"] ";
			}
			cout << std::endl;
		}
		for (int i = 0; i < rows; ++i) {
			cout << "=-";
		}
		cout << "=" << endl;
	}
	void render() {
		if (size == 2) {
			system("cls");
			for (int i = 0; i < rows; i++) {
				setConsoleColor(0, borderColor);  cout << "      ";
			}
			cout << "      " << endl;

			for (int i = 0; i < columns; i++) {
				setConsoleColor(0, borderColor);  cout << "   ";
				for (int j = 0; j < rows; j++) {
					if (matrix[i][j] == wall) { setConsoleColor(0, wallColor);  cout << "      "; }
					if (matrix[i][j] == air) { setConsoleColor(0, airColor);   cout << "      "; }
					if (matrix[i][j] == start) { setConsoleColor(0, startColor); cout << "      "; }
					if (matrix[i][j] == end) { setConsoleColor(0, endColor);   cout << "      "; }
					if (matrix[i][j] == player) {
						if (direction == Dir::UP) {
							setConsoleColor(0, playerColor);
							cout << "  ";
							setConsoleColor(0, Colors::BLACK);
							cout << "  ";
							setConsoleColor(0, playerColor);
							cout << "  ";
						}
						else {
							setConsoleColor(0, playerColor);
							cout << "      ";
						}
					}

					if (matrix[i][j] == trail) { setConsoleColor(0, pathColor);cout << "      "; }
					if (matrix[i][j] == fakeWall) { setConsoleColor(0, fakeWallColor);cout << "      "; }
				}
				setConsoleColor(0, borderColor);  cout << "   ";
				cout << endl;
				setConsoleColor(0, borderColor);  cout << "   ";
				for (int j = 0; j < rows; j++) {
					if (matrix[i][j] == wall) { setConsoleColor(0, wallColor);  cout << "      "; }
					if (matrix[i][j] == air) { setConsoleColor(0, airColor);   cout << "      "; }
					if (matrix[i][j] == start) { setConsoleColor(0, startColor); cout << "      "; }
					if (matrix[i][j] == end) { setConsoleColor(0, endColor);   cout << "      "; }
					if (matrix[i][j] == player) {
						if (direction == Dir::LEFT) {
							setConsoleColor(0, Colors::BLACK);
							cout << "  ";
							setConsoleColor(0, playerColor);
							cout << "    ";
						}
						else if (direction == Dir::RIGHT) {
							setConsoleColor(0, playerColor);
							cout << "    ";
							setConsoleColor(0, Colors::BLACK);
							cout << "  ";
						}
						else {
							setConsoleColor(0, playerColor);
							cout << "      ";
						}
					}
					if (matrix[i][j] == trail) { setConsoleColor(0, pathColor);cout << "      "; }
					if (matrix[i][j] == fakeWall) { setConsoleColor(0, fakeWallColor);cout << "      "; }
				}
				setConsoleColor(0, borderColor);  cout << "   ";
				cout << endl;
				setConsoleColor(0, borderColor);  cout << "   ";
				for (int j = 0; j < rows; j++) {
					if (matrix[i][j] == wall) { setConsoleColor(0, wallColor);  cout << "      "; }
					if (matrix[i][j] == air) { setConsoleColor(0, airColor);   cout << "      "; }
					if (matrix[i][j] == start) { setConsoleColor(0, startColor); cout << "      "; }
					if (matrix[i][j] == end) { setConsoleColor(0, endColor);   cout << "      "; }
					if (matrix[i][j] == player) {
						if (direction == Dir::DOWN) {
							setConsoleColor(0, playerColor);
							cout << "  ";
							setConsoleColor(0, Colors::BLACK);
							cout << "  ";
							setConsoleColor(0, playerColor);
							cout << "  ";
						}
						else {
							setConsoleColor(0, playerColor);
							cout << "      ";
						}
					}
					if (matrix[i][j] == trail) { setConsoleColor(0, pathColor);cout << "      "; }
					if (matrix[i][j] == fakeWall) { setConsoleColor(0, fakeWallColor);cout << "      "; }
				}
				setConsoleColor(0, borderColor);  cout << "   ";
				cout << endl;
			}
			for (int i = 0; i < rows; i++) {
				setConsoleColor(0, borderColor);  cout << "      ";
			}
			cout << "      " << endl;
			setConsoleColor(Colors::WHITE, Colors::BLACK);
		}
		else if (size == 1) {
			system("cls");
			for (int i = 0; i < rows; i++) {
				setConsoleColor(0, borderColor);  cout << "  ";
			}
			cout << "  " << endl;

			for (int i = 0; i < columns; i++) {
				setConsoleColor(0, borderColor);  cout << " ";
				for (int j = 0; j < rows; j++) {
					if (matrix[i][j] == wall) { setConsoleColor(0, wallColor);  cout << "  "; }
					if (matrix[i][j] == air) { setConsoleColor(0, airColor);   cout << "  "; }
					if (matrix[i][j] == start) { setConsoleColor(0, startColor); cout << "  "; }
					if (matrix[i][j] == end) { setConsoleColor(0, endColor);   cout << "  "; }
					if (matrix[i][j] == player) {
						if (direction == Dir::UP) {
							setConsoleColor(0, playerColor);
							cout << "  ";
						}
						else {
							setConsoleColor(0, playerColor);
							cout << "  ";
						}
					}
					if (matrix[i][j] == trail) { setConsoleColor(0, pathColor);cout << "  "; }
					if (matrix[i][j] == fakeWall) { setConsoleColor(0, fakeWallColor);cout << "  "; }
				}
				setConsoleColor(0, borderColor);  cout << " ";
				cout << endl;
			}
			for (int i = 0; i < rows; i++) {
				setConsoleColor(0, borderColor);  cout << "  ";
			}
			cout << "  " << endl;
			setConsoleColor(Colors::WHITE, Colors::BLACK);
		}

	}
	void run() {
		if (isInDeadEnd()) {
			direction = Dir::DOWN;
		}

		maxFailCount = lookAroundIgnoreDirection();
		saveCrossingsIgnoreDirections();

		do{
			explore();
		} while (path.length != 0 && failCount <= maxFailCount && found == false);

		if (found == false) {
			matrix[startPosition[0]][startPosition[1]] = start;
			render();

			cout << endl << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
				 << endl << " !!! Labirent jest niewykonalny !!! "
				 << endl << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=";
		}
		else {
			matrix[endPosition[0]][endPosition[1]] = end;
			render();

			if (path.indexOf(Dir::CROSSINGUP) != -1) {
				path.deleteElements(Dir::CROSSINGUP);
			}
			if (path.indexOf(Dir::CROSSINGDOWN) != -1) {
			path.deleteElements(Dir::CROSSINGDOWN);
			}
			if (path.indexOf(Dir::CROSSINGLEFT) != -1) {
				path.deleteElements(Dir::CROSSINGLEFT);
			}
			if (path.indexOf(Dir::CROSSINGRIGHT) != -1) {
				path.deleteElements(Dir::CROSSINGRIGHT);
			}

			showRoute();
		}

	}
	void renderAndRun() {
		render();
		wait(3);
		run();
	}
};

int main() {

	Maze maze1 = Maze("Labirynt1.txt");
	maze1.speed = 0.3;

	Maze maze2 = Maze("Labirynt2.txt");
	maze2.speed = 0.3;

	Maze maze3 = Maze("Labirynt3.txt");
	maze3.speed = 0.3;

	Maze maze4 = Maze("Labirynt4.txt");

	Maze maze5 = Maze("Labirynt5.txt");
	maze5.speed = 0.05;
	maze5.size = Maze::Size::SMALL;

	Maze maze6 = Maze("Labirynt6.txt");
	maze6.size = Maze::Size::BIG;

	/// Jeœli ustawi siê konsole na fullScreen to mo¿na ustawiæ Size na BIG
	///
	/// Program u¿ywa biblioteki windows.h wiêc jeœli program jest u¿ywany nie na Windowsie - to proszê zakomentowaæ wszystkie linijki: 
	/// - #include <windows.h>
	/// - system("cls");
	/// oce s¹ odpowiedzialne za czyszczenie konsoli.



  maze1.renderAndRun();
//	maze2.renderAndRun();
//	maze3.renderAndRun();
//	maze4.renderAndRun();
//	maze5.renderAndRun();
//	maze6.renderAndRun();


	return 0;
}

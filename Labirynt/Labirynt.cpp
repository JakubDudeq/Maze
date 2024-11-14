#include "dudqLIST.h"
#include "Maze.h"

using namespace std;

int main() {

	Maze maze1 = Maze("Labirynt1.txt");
	maze1.speed = 0.3;

//	Maze maze2 = Maze("Labirynt2.txt");
//	maze2.speed = 0.3;

//	Maze maze3 = Maze("Labirynt3.txt");
//	maze3.speed = 0.3;

//	Maze maze4 = Maze("Labirynt4.txt");

//	Maze maze5 = Maze("Labirynt5.txt");
//	maze5.speed = 0.05;
//	maze5.size = Maze::Size::SMALL;

//	Maze maze6 = Maze("Labirynt6.txt");
//	maze6.size = Maze::Size::BIG;

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

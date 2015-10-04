#include "mazeUtils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stack> 
#include <time.h>

using namespace std; 

const int SIZE = 101; 

typedef struct cell{
	int row;
	int col; 
	bool visited; 
	bool blocked; 
};

void printMaze(cell** maze, int h, int w){
	for (int iRow = 0; iRow < h; iRow++){
		for (int iCol = 0; iCol < w; iCol++){
			//cout << maze[iRow][iCol].blocked;
			cout << (maze[iRow][iCol].blocked ? "x  " : "o  ");
		}
		cout << "\n";
	}
}

void saveMaze(cell** maze, int h, int w, int id){
	ofstream outfile;
	ostringstream os; 
	os << "mazes/maze" << id << ".txt";
	string filename = os.str();
	outfile.open(filename.c_str());
	for (int iRow = 0; iRow < h; iRow++){
		for (int iCol = 0; iCol < w; iCol++){
			//cout << maze[iRow][iCol].blocked;
			outfile << (maze[iRow][iCol].blocked ? "x" : "o");
		}
		outfile << "\n";
	}
	outfile.close();
}

void generateMaze(int id){
	cell **maze;
	maze = new cell *[SIZE];
	for (int i = 0; i < SIZE; i++){
		maze[i] = new cell[SIZE];
	}

	// init
	for (int i = 0; i < SIZE; i++){
		for (int j = 0; j < SIZE; j++){
			maze[i][j].row = i;
			maze[i][j].col = j;
			maze[i][j].visited = false;
			maze[i][j].blocked = false;
		}
	}


	int nVisited = 0; 
	
	while (nVisited < SIZE * SIZE){
		// randomly choose the starting point 
		int r = rand() % SIZE;
		int c = rand() % SIZE;
		if (maze[r][c].visited) continue;
		cout << "r: " << r << ", c: " << c << endl;

		// DFS 
		stack<cell> S;
		S.push(maze[r][c]);
		while (!S.empty()){
			cell u = S.top();
			S.pop();
			if (maze[u.row][u.col].visited == false){
				maze[u.row][u.col].visited = true;
				nVisited++; 

				// Set blocked with probability 
				maze[u.row][u.col].blocked = (rand() % 100) < 30;

				// Check left 
				if (u.col >= 1 && !maze[u.row][u.col - 1].visited)
					S.push(maze[u.row][u.col - 1]);

				// Check right 
				if (u.col < SIZE - 1 && !maze[u.row][u.col + 1].visited)
					S.push(maze[u.row][u.col - 1]);

				// Check up 
				if (u.row >= 1 && !maze[u.row - 1][u.col].visited)
					S.push(maze[u.row - 1][u.col]);

				// Check down 
				if (u.row < SIZE - 1 && !maze[u.row + 1][u.col].visited)
					S.push(maze[u.row + 1][u.col]);

			}
		}
	}
	// printMaze(maze, SIZE, SIZE);		
	saveMaze(maze, SIZE, SIZE, id);
}

void generateMazes(int nMazes){

	srand(time(NULL));
	for (int i = 1; i <= nMazes; i++){
		generateMaze(i);
	}
}

bool** loadMaze(int id){
	ifstream ifs;
	ostringstream os;
	os << "C:/Users/Viet Nguyen/code/cs520-assignment1/x64/Debug/mazes/maze" << id << ".txt";
	//os << "mazes/maze" << id << ".txt";
	string filename = os.str();
	ifs.open(filename.c_str(), std::ifstream::in);

	bool** maze = new bool*[SIZE];
	for (int i = 0; i < SIZE; i++){
		maze[i] = new bool[SIZE];
	}

	int row = 0; 
	int col = 0;
	char c = ifs.get();
	while (ifs.good()){
		if (c == 'x'){
			maze[row][col] = false;
			col++;
		}
		else if (c == 'o'){
			maze[row][col] = true;
			col++;
		}
		else if (c == '\n'){
			row++;
			col = 0;
		}
		//cout << c;
		c = ifs.get();
	}


	/*
	for (int iRow = 0; iRow < h; iRow++){
		for (int iCol = 0; iCol < w; iCol++){
			//cout << maze[iRow][iCol].blocked;
			outfile << (maze[iRow][iCol].blocked ? "x  " : "o  ");
		}
		outfile << "\n";
	}
	*/
	ifs.close();
	return maze;

}

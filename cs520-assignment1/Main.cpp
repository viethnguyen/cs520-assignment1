#include <iostream>
#include <list>
#include "mazeUtils.h"
#include "binaryHeap.h"

using namespace std;

const int INF = 65536;

void computePath(State** S, bool** maze, State *goal, BinaryHeap* OPEN, list<State>* CLOSED, int counter, int size){
	while (goal->g > OPEN->extractMin.f){
		State s = OPEN->extractMin;
		OPEN->deleteMin;
		CLOSED->push_back(s);
		//4 possible moves: left, right, up, down
		int incRows[] = { 0, 0, -1, 1 };
		int incCols[] = { -1, 1, 0, 0 };
		for (int i = 0; i < 4; i++){
			int incRow = incRows[i];
			int incCol = incCols[i];
			int newRow = s.row + incRow;
			int newCol = s.row + incCol; 

			// check if valid 
			if (newRow < 0 || newRow >= size || newCol < 0 || newCol >= size){
				continue;
			}

			if (S[newRow][newCol].search < counter){
				S[newRow][newCol].g = INF;
				S[newRow][newCol].search = counter;
			}

			if (maze[newRow][newCol] == false){
				s.cost[i] = INF;
			}

			if (S[newRow][newCol].g > s.g + s.cost[i]){

			}

		}
	}
}

void repeatedForwardAStar(bool** maze, int size, State start, State goal){
	int counter = 0;
	State** S = new State*[size];
	for (int i = 0; i < size; i++){
		S[i] = new State[size];
	}
	for (int r = 0; r < size; r++){
		for (int c = 0; c < size; c++){
			S[r][c].row = r;
			S[r][c].col = c;
			S[r][c].search = 0;
		}
	}

	while (!compareStatePos(&start, &goal)){
		counter = counter + 1;
		start.g = 0;
		start.h = manhattanDistance(&start, &goal);
		start.f = start.g + start.h;
		start.search = counter;
		goal.g = INF;
		goal.search = counter; 
		BinaryHeap OPEN;
		list<State> CLOSED;
		OPEN.insert(start);
		computePath(S, maze, &goal, &OPEN, &CLOSED, counter, size);
		if (OPEN.size() == 0){
			cout << "I cannot reach the target\n";
			return;
		}
		
	}
	// check maze 
	//for (int r = 0; r < size; r++){
	//	for (int c = 0; c < size; c++){
	//		if (maze[r][c] == true)
	//			cout << "o";
	//		else
	//			cout << "x";
	//	}
	//	cout << "\n";
	//}


	// clean up 
	for (int i = 0; i < size; i++){
		delete[] S[i];
	}
	delete[] S;
}

int main(){
	int counter = 0; 
	// testStateBinaryHeap();

	bool** maze = loadMaze(1);
	State start;
	start.row = 0;
	start.col = 4;
	State goal;
	goal.row = 1;
	goal.col = 0;
	repeatedForwardAStar(maze, 101, start, goal);
	
	// generateMazes(50);
	return 0; 
}
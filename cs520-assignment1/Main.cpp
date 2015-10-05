#include <iostream>
#include <list>
#include <stack>
#include "mazeUtils.h"
#include "binaryHeap.h"

using namespace std;

const int INF = 65536;

void computePath(State** S, bool** maze, State *goal, BinaryHeap* OPEN, list<State>* CLOSED, int counter, int size){
	cout << "Running computePath..." << endl;
	while (OPEN->size() != 0 && S[goal->row][goal->col].g > (OPEN->extractMin()).f){
		State s = OPEN->extractMin();
		int oldRow = s.row;
		int oldCol = s.col;
		cout << "Investigating: (" << oldRow << ", " << oldCol << ")" << endl;
		OPEN->deleteMin();
		CLOSED->push_back(s);
		//4 possible moves: left, right, up, down
		int incRows[] = { 0, 0, -1, 1 };
		int incCols[] = { -1, 1, 0, 0 };
		for (int i = 0; i < 4; i++){
			int incRow = incRows[i];
			int incCol = incCols[i];
			int newRow = s.row + incRow;
			int newCol = s.col + incCol; 

			// check if valid 
			if (newRow < 0 || newRow >= size || newCol < 0 || newCol >= size){
				continue;
			}

			if (S[newRow][newCol].search < counter){
				S[newRow][newCol].g = INF;
				S[newRow][newCol].search = counter;
			}

			if (maze[newRow][newCol] == false){
				S[oldRow][oldCol].cost[i] = INF;
			}

			if (S[newRow][newCol].g > S[oldRow][oldCol].g + S[oldRow][oldCol].cost[i]){
				S[newRow][newCol].g = S[oldRow][oldCol].g + S[oldRow][oldCol].cost[i];
				//S[newRow][newCol].tree = &s;
				S[newRow][newCol].treeRow = s.row;
				S[newRow][newCol].treeCol = s.col;
				int index = OPEN->findStateByPosition(newRow, newCol);
				if (index != -1){
					OPEN->modifyF(index, S[newRow][newCol].g + manhattanDistance(&S[newRow][newCol], goal));  
				}
				else{
					S[newRow][newCol].f = S[newRow][newCol].g + manhattanDistance(&S[newRow][newCol], goal);
					OPEN->insert(S[newRow][newCol]);
				}
			}

		}
	}
}


struct coord{
	int row;
	int col;
};

// 0: left, 1: right, 2: up, 3: down; -1: invalid
int direction(coord* from, coord* to){
	if (from->row == to->row){
		if (from->col == to->col - 1){
			return 1;
		}
		if (from->col == to->col + 1){
			return 0;
		}
	}
	else if (from->col == to->col){
		if (from->row == to->row - 1){
			return 3;
		}
		if (from->row == to->row + 1){
			return 2;
		}
	}
	return -1;
}


void repeatedForwardAStar(bool** maze, int size, State* start, State* goal){
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

	while (!compareStatePos(start, goal)){
		counter = counter + 1;
		S[start->row][start->col].g = 0;
		S[start->row][start->col].h = manhattanDistance(start, goal);
		S[start->row][start->col].f = start->g + start->h;
		S[start->row][start->col].search = counter;
		S[goal->row][goal->col].g = INF;
		S[goal->row][goal->col].search = counter;
		BinaryHeap OPEN;
		list<State> CLOSED;
		OPEN.insert(S[start->row][start->col]);
		computePath(S, maze, goal, &OPEN, &CLOSED, counter, size);
		if (OPEN.size() == 0){
			cout << "I cannot reach the target\n";

			// clean up 
			for (int i = 0; i < size; i++){
				delete[] S[i];
			}
			delete[] S;

			return;
		}

		// follow the tree-pointers from sgoal to start 
		stack<coord> path;
		coord curCoord;
		curCoord.row = goal->row;
		curCoord.col = goal->col;
		path.push(curCoord);

		while (curCoord.row != start->row || curCoord.col != start->col){
			int curRow = curCoord.row;
			int curCol = curCoord.col;
			curCoord.row = S[curRow][curCol].treeRow;
			curCoord.col = S[curRow][curCol].treeCol;
			path.push(curCoord);
		}

		//State* tmp = &S[goal->row][goal->col];
		//while (!compareStatePos(tmp, &S[start->row][start->col])){
		//	tmp = tmp->tree;
		//	curCoord.row = tmp->row;
		//	curCoord.col = tmp->col;
		//	path.push(curCoord);
		//}
		//delete tmp;
		
		//move the agent along the resulting path from sstart to sgoal 
		coord current = path.top();
		path.pop();
		while (!path.empty()){
			coord next = path.top();
			path.pop();
			cout << "\tMove to: (" << next.row << ", " << next.col << ")" << endl;
			int dir = direction(&current, &next);
			if (S[current.row][current.col].cost[dir] > 1){
				start = &S[current.row][current.col];
				cout << "\tMove start to: (" << current.row << ", " << current.col << ")" << endl;
				break;
			}
			current = next;
		}
		if (path.empty()){
			start = &S[current.row][current.col];
		}

	}

	cout << "I reached the target" << endl;
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
	start.row = 1;
	start.col = 0;
	State goal;
	goal.row = 60;
	goal.col = 60;
	if (maze[start.row][start.col] == false || maze[goal.row][goal.col] == false){
		cout << "Invalid settings" << endl;
		return -1;
	}
	repeatedForwardAStar(maze, 101, &start, &goal);
	
	// generateMazes(50);

	for (int i = 0; i < 101; i++){
		delete[] maze[i];
	}
	delete[] maze;
	return 0; 
}
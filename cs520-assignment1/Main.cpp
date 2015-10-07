#include <iostream>
#include <list>
#include <stack>
#include "mazeUtils.h"
#include "binaryHeap.h"

using namespace std;

const int INF = 65536;
const int LEFT = 0;
const int RIGHT = 1;
const int UP = 2;
const int DOWN = 3;

int nExpandedStates = 0;

void computePath(State** S, bool** maze, State *goal, BinaryHeap* OPEN, list<State>* CLOSED, int counter, int size){
	cout << "Running computePath..." << endl;
	while (OPEN->size() != 0 && S[goal->row][goal->col].g > (OPEN->extractMin()).f){
		State s = OPEN->extractMin();
		int oldRow = s.row;
		int oldCol = s.col;
		cout << "Expand state: (" << oldRow << ", " << oldCol << ")" << endl;
		nExpandedStates++;
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

			if (S[newRow][newCol].g > S[oldRow][oldCol].g + S[oldRow][oldCol].cost[i]){
				S[newRow][newCol].g = S[oldRow][oldCol].g + S[oldRow][oldCol].cost[i];
				S[newRow][newCol].treeRow = s.row;
				S[newRow][newCol].treeCol = s.col;
				int index = OPEN->findStateByPosition(newRow, newCol);
				if (index != -1){
					if (S[newRow][newCol].h == 0){
						S[newRow][newCol].h = manhattanDistance(&S[newRow][newCol], goal);
					}
					OPEN->modifyF(index, S[newRow][newCol].g + S[newRow][newCol].h);
					OPEN->heapifyup(index);
				}
				else{
					if (S[newRow][newCol].h == 0){
						S[newRow][newCol].h = manhattanDistance(&S[newRow][newCol], goal);
					}
					S[newRow][newCol].f = S[newRow][newCol].g + S[newRow][newCol].h;
					OPEN->insert(S[newRow][newCol]);
				}
			}

		}
	}

	// Adaptive A*: Update h 
	list<State>::iterator it;
	for (it = CLOSED->begin(); it != CLOSED->end(); it++){
		S[it->row][it->col].h = S[goal->row][goal->col].g - S[it->row][it->col].g;
	}
}


struct coord{	
	int row;
	int col;
};

// Suppose this state (row, col) is blocked, update all cost going into and out of this state to INF
void updateCostToInf(State** S, int size, int row, int col){
	if (row > 0){
		S[row - 1][col].cost[DOWN] = INF;
		S[row][col].cost[UP] = INF;
	}
	if (row < size - 1){
		S[row + 1][col].cost[UP] = INF;
		S[row][col].cost[DOWN] = INF;
	}
	if (col > 0){
		S[row][col - 1].cost[RIGHT] = INF;
		S[row][col].cost[LEFT] = INF;
	}
	if (col < size - 1){
		S[row][col + 1].cost[LEFT] = INF;
		S[row][col].cost[RIGHT] = INF;
	}
}

// 0: left, 1: right, 2: up, 3: down; -1: invalid
int direction(coord* from, coord* to){
	if (from->row == to->row){
		if (from->col == to->col - 1){
			return RIGHT;
		}
		if (from->col == to->col + 1){
			return LEFT;
		}
	}
	else if (from->col == to->col){
		if (from->row == to->row - 1){
			return DOWN;
		}
		if (from->row == to->row + 1){
			return UP;
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

	//S[goal->row][goal->col].g = INF;

	while (!compareStatePos(start, goal)){
		S[goal->row][goal->col].g = INF;
		S[goal->row][goal->col].search = counter;

		counter = counter + 1;
		S[start->row][start->col].g = 0;
		S[start->row][start->col].h = manhattanDistance(start, goal);
		//S[start->row][start->col].h = S[goal->row][goal->col].g - S[start->row][start->col].g;
		S[start->row][start->col].f = start->g + start->h;
		S[start->row][start->col].search = counter;
		
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

		//move the agent along the resulting path from sstart to sgoal 
		coord current = path.top();
		path.pop();
		while (!path.empty()){
			// watch 4 states around this current state to update cost, if any 
			if (current.row > 0 && !maze[current.row - 1][current.col]){
				updateCostToInf(S, size, current.row - 1, current.col);
			}
			if (current.row < size - 1 && !maze[current.row + 1][current.col]){
				updateCostToInf(S, size, current.row + 1, current.col);
			}
			if (current.col > 0 && !maze[current.row][current.col-1]){	
				updateCostToInf(S, size, current.row, current.col-1);
			}
			if (current.col < size - 1 && !maze[current.row][current.col+1]){
				updateCostToInf(S, size, current.row, current.col+1);
			}

			coord next = path.top();
			path.pop();
			cout << "\tTried to move to: (" << next.row << ", " << next.col << ")... " ;
			int dir = direction(&current, &next);
			if (S[current.row][current.col].cost[dir] > 1){
				cout << "BLOCKED" << endl;
				start = &S[current.row][current.col];
				break;
			}
			cout << "OK! Moved start to: (" << next.row << ", " << next.col << ")" << endl;
			current = next;
		}
		if (path.empty()){
			start = &S[current.row][current.col];
		}

	}

	cout << "I reached the target" << endl;


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
	start.col = 2;
	State goal;
	goal.row = 25;
	goal.col = 25;
	if (maze[start.row][start.col] == false || maze[goal.row][goal.col] == false){
		cout << "Invalid settings" << endl;
		return -1;
	}
	repeatedForwardAStar(maze, 101, &start, &goal);
	
	// generateMazes(50);

	cout << "Number of expanded states: " << nExpandedStates << endl;
	for (int i = 0; i < 101; i++){
		delete[] maze[i];
	}
	delete[] maze;
	return 0; 
}
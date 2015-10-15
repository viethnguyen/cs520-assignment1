#include <iostream>
#include <list>
#include <stack>
#include <sstream>
#include <fstream>
#include <time.h>
#include "mazeUtils.h"
#include "binaryHeap.h"

using namespace std;

const int INF = 65536;
const int LEFT = 0;
const int RIGHT = 1;
const int UP = 2;
const int DOWN = 3;

int nExpandedStates = 0;

void computePath(State** S, bool** maze, State *goal, BinaryHeap* OPEN, list<State>* CLOSED, int counter, int size, bool adaptive){
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
	if (adaptive){
		list<State>::iterator it;
		for (it = CLOSED->begin(); it != CLOSED->end(); it++){
		S[it->row][it->col].h = S[goal->row][goal->col].g - S[it->row][it->col].g;
		}
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
	}
	if (row < size - 1){
		S[row + 1][col].cost[UP] = INF;
	}
	if (col > 0){
		S[row][col - 1].cost[RIGHT] = INF;
	}
	if (col < size - 1){
		S[row][col + 1].cost[LEFT] = INF;
	}

	for (int i = 0; i < 4; i++){
		S[row][col].cost[i] = INF;
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

void repeatedForwardAStar(bool** maze, int size, State* start, State* goal, bool adaptive){
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
		S[goal->row][goal->col].g = INF;
		S[goal->row][goal->col].search = counter;

		counter = counter + 1;
		S[start->row][start->col].g = 0;
		S[start->row][start->col].h = manhattanDistance(start, goal);
		S[start->row][start->col].f = start->g + start->h;
		S[start->row][start->col].search = counter;
		
		BinaryHeap OPEN;
		list<State> CLOSED;

		OPEN.insert(S[start->row][start->col]);
		computePath(S, maze, goal, &OPEN, &CLOSED, counter, size, adaptive);

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

void repeatedBackwardAStar(bool** maze, int size, State* start, State* goal){
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

		S[start->row][start->col].g = INF;
		S[start->row][start->col].search = counter;

	
		S[goal->row][goal->col].g = 0;
		S[goal->row][goal->col].h = manhattanDistance(start, goal);
		S[goal->row][goal->col].f = goal->g + goal->h;
		S[goal->row][goal->col].search = counter;

		BinaryHeap OPEN;
		list<State> CLOSED;

		// watch 4 states around the start state to update cost, if any 
		if (start->row > 0 && !maze[start->row - 1][start->col]){
			updateCostToInf(S, size, start->row - 1, start->col);
		}
		if (start->row< size - 1 && !maze[start->row + 1][start->col]){
			updateCostToInf(S, size, start->row + 1, start->col);
		}
		if (start->col > 0 && !maze[start->row][start->col - 1]){
			updateCostToInf(S, size, start->row, start->col - 1);
		}
		if (start->col < size - 1 && !maze[start->row][start->col + 1]){
			updateCostToInf(S, size, start->row, start->col + 1);
		}

		OPEN.insert(S[goal->row][goal->col]);
		computePath(S, maze, start, &OPEN, &CLOSED, counter, size, false);

		if (OPEN.size() == 0){
			cout << "I cannot reach the target\n";

			// clean up 
			for (int i = 0; i < size; i++){
				delete[] S[i];
			}
			delete[] S;

			return;
		}

		// follow the tree-pointers from sstart to sgoal 
		//stack<coord> path;
		coord current;
		current.row = start->row;
		current.col = start->col;
		//path.push(curCoord);

		while (current.row != goal->row || current.col != goal->col){
			int curRow = current.row;
			int curCol = current.col;
			
			// watch 4 states around this current state to update cost, if any 
			if (current.row > 0 && !maze[current.row - 1][current.col]){
				updateCostToInf(S, size, current.row - 1, current.col);
			}
			if (current.row < size - 1 && !maze[current.row + 1][current.col]){
				updateCostToInf(S, size, current.row + 1, current.col);
			}
			if (current.col > 0 && !maze[current.row][current.col - 1]){
				updateCostToInf(S, size, current.row, current.col - 1);
			}
			if (current.col < size - 1 && !maze[current.row][current.col + 1]){
				updateCostToInf(S, size, current.row, current.col + 1);
			}

			coord next; 

			next.row = S[curRow][curCol].treeRow;
			next.col = S[curRow][curCol].treeCol;
			int dir = direction(&current, &next);
			cout << "\tTried to move to: (" << next.row << ", " << next.col << ")... ";
			if (S[current.row][current.col].cost[dir] > 1){
				cout << "BLOCKED" << endl;
				start = &S[current.row][current.col];
				break;
			}
			cout << "OK! Moved start to: (" << next.row << ", " << next.col << ")" << endl;
			current = next;
		}
		start = &S[current.row][current.col];
	}

	cout << "I reached the target" << endl;


	// clean up 
	for (int i = 0; i < size; i++){
		delete[] S[i];
	}
	delete[] S;
}

void printM(bool** maze, int h, int w){
	for (int iRow = 0; iRow < h; iRow++){
		for (int iCol = 0; iCol < w; iCol++){
			//cout << maze[iRow][iCol].blocked;
			cout << (maze[iRow][iCol] ? "x" : "o");
		}
		cout << "\n";
	}
}
int main(){
	//ofstream outfile;
	//ostringstream os;
	//os << "C:/Users/Viet Nguyen/code/cs520-assignment1/x64/Debug/compareAdaptive.txt";
	//string filename = os.str();
	//outfile.open(filename.c_str());

	int nTrials = 1;
	int nSuccessTrials = 0; 

	srand(time(NULL));
	int SIZE = 101;

	int choice; 
	cout << "Select function:" << endl;
	cout << "1. Print gridworld environment. " << endl;
	cout << "2. Repeated Forward A*" << endl;
	cout << "3. Repeated Backward A*" << endl;
	cout << "4. Adaptive A*" << endl;
	cout << "5. Exit" << endl;
	cout << "Your choice: "; 
	cin >> choice; 

	int mazeNumber;
	cout << "Maze number: ";
	cin >> mazeNumber;
	bool** maze = loadMaze(mazeNumber);

	State start, goal;

	switch (choice){
	case 1:
		printM(maze, SIZE, SIZE);
		break;
	case 2:
		cout << "Start row: "; 
		cin >> start.row;
		cout << "Start col:";
		cin >> start.col;
		cout << "Goal row: ";
		cin >> goal.row;
		cout << "Goal col: ";
		cin >> goal.col;
		
		if (maze[start.row][start.col] == false || maze[goal.row][goal.col] == false){
			cout << "Invalid: Either start state or end state is blocked." << endl;
			break;
		}
		nExpandedStates = 0;
		repeatedForwardAStar(maze, SIZE, &start, &goal, false);
		cout  << "Find path from (" << start.row << "," << start.col << ") to (" << goal.row << "," << goal.col << ")" << endl;
		cout  << "Repeated Forward A*: Number of expanded states: " << nExpandedStates << endl;
		break;
	case 3:
		cout << "Start row: ";
		cin >> start.row;
		cout << "Start col:";
		cin >> start.col;
		cout << "Goal row: ";
		cin >> goal.row;
		cout << "Goal col: ";
		cin >> goal.col;

		if (maze[start.row][start.col] == false || maze[goal.row][goal.col] == false){
			cout << "Invalid: Either start state or end state is blocked." << endl;
			break;
		}
		nExpandedStates = 0;
		repeatedBackwardAStar(maze, SIZE, &start, &goal);
		cout << "Find path from (" << start.row << "," << start.col << ") to (" << goal.row << "," << goal.col << ")" << endl;
		cout << "Repeated Backward A*: Number of expanded states: " << nExpandedStates << endl;
		break;
	case 4:
		cout << "Start row: ";
		cin >> start.row;
		cout << "Start col:";
		cin >> start.col;
		cout << "Goal row: ";
		cin >> goal.row;
		cout << "Goal col: ";
		cin >> goal.col;

		if (maze[start.row][start.col] == false || maze[goal.row][goal.col] == false){
			cout << "Invalid: Either start state or end state is blocked." << endl;
			break;
		}
		nExpandedStates = 0;
		repeatedForwardAStar(maze, SIZE, &start, &goal, true);
		cout << "Find path from (" << start.row << "," << start.col << ") to (" << goal.row << "," << goal.col << ")" << endl;
		cout << "Adaptive A*: Number of expanded states: " << nExpandedStates << endl;
		break;
	case 5:
		break;
	default:
		break;
	}

	//while (nSuccessTrials < nTrials){
	//	State start, goal;
	//	//start.row = rand() % SIZE;
	//	//start.col = rand() % SIZE;
	//	//goal.row = rand() % SIZE;
	//	//goal.col = rand() % SIZE;
	//	start.row = 71;
	//	start.col =	 51;
	//	goal.row = 65;
	//	goal.col = 96;
	//	//outfile << "=== Trial " << nSuccessTrials + 1 << " ===" << endl;
	//	//outfile << "Find path from (" << start.row << "," << start.col << ") to (" << goal.row << "," << goal.col << ")" << endl;
	//	if (maze[start.row][start.col] == false || maze[goal.row][goal.col] == false){
	//		cout  << "Invalid: Either start state or end state is blocked." << endl;
	//		continue;
	//	}
	//	//nExpandedStates = 0;
	//	//repeatedForwardAStar(maze, SIZE, &start, &goal, false);
	//	//outfile << "Repeated Forward A*: Number of expanded states: " << nExpandedStates << endl;
	//	
	//	nExpandedStates = 0;
	//	repeatedForwardAStar(maze, SIZE, &start, &goal, false);
	//	cout << "Repeated Forward A*: Number of expanded states: " << nExpandedStates << endl;
	//	//outfile << "Adaptive A*: Number of expanded states: " << nExpandedStates << endl;

	//	nSuccessTrials++;
	//}

	// clean up 
	//outfile.close();

	for (int i = 0; i < 101; i++){
		delete[] maze[i];
	}
	delete[] maze;
	return 0; 
}
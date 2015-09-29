#include <iostream>
#include "mazeUtils.h"

using namespace std;

typedef struct State{
	int row;
	int col;
};

int main(){
	int counter = 0; 
	//for testing 
	State start; 
	start.row = 5;
	start.col = 3;
	State goal; 
	goal.row = 5;
	goal.col = 5; 

	generateMazes(50);
	//loadMaze(20);
	return 0; 
}
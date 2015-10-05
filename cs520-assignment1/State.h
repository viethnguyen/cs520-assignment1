#ifndef __STATE_H__
#define __STATE_H__

#include <iostream>

using namespace std;

class State{
public:
	int row;
	int col;
	int g;	// the length of the shortest path from the start state 
	int h;  // heuristic, estimates the goal distance 
	int f;  // f = g+h
	State* tree;
	int treeRow;
	int treeCol;
	int search;
	int cost[4]; // cost of 4 actions: left, right, up, down

	State(){
		row = 0;
		col = 0;
		g = 0;
		h = 0;
		f = 0;
		search = 0;
		treeRow = 0;
		treeCol = 0;
		cost[0] = cost[1] = cost[2] = cost[3] = 1;
	}
	void setF(int newF){
		f = newF;
	}
	void print(){
		cout << "State: " << endl;
		cout << "\trow: " << row << endl;
		cout << "\tcol: " << col << endl;
		cout << "\tg: " << g << endl;
		cout << "\th: " << h << endl;
		cout << "\tf: " << f << endl;
		cout << "\tsearch: " << search << endl;
	}
};

State nullState(){
	State s;
	s.setF(-1);
	return s;
}

bool compareStateF(State* a, State* b){
	return a->f > b->f;
}

bool compareStatePos(State* a, State* b){
	return (a->col == b->col && a->row == b->row);
}

int manhattanDistance(State* a, State* b){
	return (abs(a->col - b->col) + abs(a->row - b->row));
}

#endif // __STATE_H__
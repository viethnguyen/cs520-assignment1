#ifndef __BINARY_HEAP_H__
#define __BINARY_HEAP_H__

#include <iostream>
#include <vector>
#include <cstdlib>
#include <iterator>
#include "State.h"

using namespace std;



class BinaryHeap{
private:
	vector<State> heap;
	int left(int parent);
	int right(int parent);
	int parent(int child);

public:
	void heapifyup(int index);
	void heapifydown(int index);
	BinaryHeap(){}
	void insert(State element);
	void deleteMin();
	State extractMin();
	void displayHeap();
	int size();
	int findStateByPosition(int row, int col);
	void modifyF(int index, int f);
};

int BinaryHeap::size(){
	return heap.size();
}

void BinaryHeap::insert(State element){
	heap.push_back(element);
	heapifyup(heap.size() - 1);
}

void BinaryHeap::deleteMin(){
	if (heap.size() == 0)
	{
		cout << "Heap is empty!" << endl;
		return;
	}
	heap[0] = heap.at(heap.size() - 1);
	heap.pop_back();
	heapifydown(0);
	cout << "Element deleted" << endl;
}

State BinaryHeap::extractMin(){
	if (heap.size() == 0){
		return nullState();
	}
	else
		return heap.front();
}

void BinaryHeap::displayHeap(){
	vector<State>::iterator pos = heap.begin();
	cout << "Heap -->  ";
	while (pos != heap.end()){
		cout << pos->f << " ";
		pos++;
	}
	cout << endl;
}

int BinaryHeap::left(int parent)
{
	int l = 2 * parent + 1;
	if (l < heap.size())
		return l;
	else
		return -1;
}


int BinaryHeap::right(int parent)
{
	int r = 2 * parent + 2;
	if (r < heap.size())
		return r;
	else
		return -1;
}


int BinaryHeap::parent(int child)
{
	int p = (child - 1) / 2;
	if (child == 0)
		return -1;
	else
		return p;
}

void BinaryHeap::heapifyup(int in)
{
	if (in >= 0 && parent(in) >= 0 && compareStateF(&heap[parent(in)], &heap[in])) //heap[parent(in)] > heap[in])
	{
		State temp = heap[in];
		heap[in] = heap[parent(in)];
		heap[parent(in)] = temp;
		heapifyup(parent(in));
	}
}

void BinaryHeap::heapifydown(int in)
{
	int child = left(in);
	int child1 = right(in);
	if (child >= 0 && child1 >= 0 && compareStateF(&heap[child], &heap[child1])){// heap[child] > heap[child1]){
		child = child1;
	}
	if (child > 0){
		State temp = heap[in];
		heap[in] = heap[child];
		heap[child] = temp;
		heapifydown(child);
	}
}

int BinaryHeap::findStateByPosition(int row, int col){
	vector<State>::iterator it = heap.begin();
	int index = 0;
	while (it != heap.end()){
		if (it->row == row && it->col == col){
			return index;
		}
		it++;
	}
	return -1; 
}

void BinaryHeap::modifyF(int index, int f){
	heap[index].setF(f);
	heapifyup(index);
}

int testStateBinaryHeap(){
	BinaryHeap h;
	State element;
	element.f = 10;
	h.insert(element);
	element.f = 11;
	h.insert(element);
	element.f = 12;
	h.insert(element);
	element.f = 8;
	h.insert(element);
	h.displayHeap();
	h.deleteMin();
	h.displayHeap();
	State s = h.extractMin();
	s.print();
	h.deleteMin();
	h.displayHeap();
	return 0;
}

#endif // __BINARY_HEAP_H__
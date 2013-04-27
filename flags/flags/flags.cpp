#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;

class Point {
public:
	Point(int x, int y, string name);
	~Point();
	void print();

private:
	int x;
	int y;
	string name;
};

Point::Point(int x, int y, string name) {
	this->x = x;
	this->y = y;
	this->name = name;
}

Point::~Point() { }

void Point::print() {
	cout << "X    : " << this->x << endl;
	cout << "Y    : " << this->y << endl;
	cout << "Name : " << this->name << endl << endl;
}

int _tmain(int argc, _TCHAR* argv[]) {

	Point a(0,0,"aaa");
	Point *b = new Point(1,1,"bbb");
	a.print();
	b->print();

	getchar();
	return 0;
}
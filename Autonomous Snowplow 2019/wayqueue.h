#ifndef _WAY_POINT_LIST
#define _WAY_POINT_LIST

#include <cstddef>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

struct Point {
	double x, y;
	Point *next;
};

class Wayqueue {
private:
	atomic<int> size;
	Point *head, *tail;
	std::mutex mtx;
	condition_variable cond_var;
public:
	Wayqueue();
	void push(Point *toAdd);
	Point* pop();
	int getSize();
	void clearAll();
	void display();
	~Wayqueue() {};
};

#endif

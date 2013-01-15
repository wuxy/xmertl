#include <stdio.h>

class A {
protected:
	int a;
public:
	virtual void print()=0;
	A() { a = 1; printf("initializing A\n");}
	virtual ~A() { printf("uninitializing A\n"); }
};

class B : public A
{
public:
	B() { a = 1; printf("initializing B\n");}
	~B() { printf("uninitializing B\n");}
	virtual void print() { printf("B::print: %d\n", a); }
};

B a, c;
A *ptr_b;

int main () {
  ptr_b = new B;
  ptr_b -> print();
  delete ptr_b;
  return 0;
}



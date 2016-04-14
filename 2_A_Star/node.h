#ifndef NODE_H
#define NODE_H
#include <vector>
#include <iostream>
using std::vector;
using std::ostream;
class node
{
	int data[3][3];
public:
	int g;
	node();
	node(int**);
	node(const node&);
	void operator=(const node&);
	node* last;
	vector<node> expand() const;
	int cal() const;
	friend bool operator<(const node&,const node&);
	friend bool operator==(const node&,const node&);
	friend ostream& operator<<(ostream& o,const node&);
};

#endif
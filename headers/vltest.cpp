#include "vectorlock.hpp"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace wam;

int main()
{
	VectorLock<string> vl;
	unsigned i1 = vl.reserve();
	unsigned i2 = vl.reserve();
	unsigned i3 = vl.reserve();
	vl.push_back(i3, string("world"));
	vl.push_back(i1, string("first"));
	vl.push_back(i2, string("hello"));

	vector<string> v = vl.getVector();

	for(vector<string>::iterator it = v.begin();
				it != v.end()
				; ++it)
		cout << *it << endl;
}

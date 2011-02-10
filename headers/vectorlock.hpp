#ifndef VECTORLOCK_HPP
#define VECTORLOCK_HPP

#include <vector>
#include <queue>
#include <map>
#include <limits>

#include <iostream>

//namespace shoudl eventually be different... it is it's own tool.
//however it NEEDS a namespace...
namespace wam
{
	using namespace std;

	//in the future their should be an optional template parameter for the size of QueueIndex. currently it's 255... a user may want more space than that.
	template<typename T>
	class VectorLock
	{
		public:
		typedef unsigned char QueueIndex;
		//try typedef typename T::iterator Iterator;
		//ref: http://pages.cs.wisc.edu/~driscoll/typename.html
		typedef __gnu_cxx::__normal_iterator<T*, std::vector<T, std::allocator<T> > > Iterator;
	
		private:
		vector<T> thevector;
	
		QueueIndex index;
		queue<QueueIndex> indexPriority;
		map<QueueIndex, vector<T> > buffer;

		void update()
		{
			QueueIndex topPriority;
			while((buffer.find(topPriority = indexPriority.front())) 
					  	!= buffer.end())
			{
				indexPriority.pop();
				thevector.insert(thevector.end(), buffer[topPriority].begin()
							, buffer[topPriority].end());
				buffer.erase(topPriority);
			}
		}

		unsigned newIndex()
		{
			if(index == numeric_limits<QueueIndex>::max())
				index = 0;
			else
				++index;

			if(indexPriority.front() == index)
				std::cerr << "you have too many things on priority in vector lock!";

			return index;
		}
	
		public:
		//let the default constructor do it's thing.
		//apparently the default constructor is retarded with unsigned chars
		VectorLock()
			:thevector(), index(0), indexPriority(), buffer()
		{}
	
		unsigned reserve()
		{
			//this is jsut to be sure....
			unsigned i;
			//
			indexPriority.push(i = newIndex());
			return i;
		}

		void push_back(QueueIndex i, T item)
		{
			buffer[i] = vector<T>(1, item);
			update();
		}

		//always inserts at the end.
		//I tried making this generic...
		//HERE
		void append(QueueIndex i
			, Iterator begin, Iterator end)
		{
			buffer[i] = vector<T>(begin, end);
			update();
		}

		vector<T> getVector()
		{
			if(!bufferEmpty())
				std::cerr << "your buffer isn't empty... you cannot get the vector.\n If you still want to try, then wrap the getVector in an if statement and check for bufferEmpty() == true.";

			return thevector;
		}

		bool bufferEmpty() { return indexPriority.empty(); }

		void clear() { 
			index = 0;
			thevector.clear(); 
			buffer.clear(); 
			while(!indexPriority.empty())
				indexPriority.pop();
		}
	};
}	
#endif

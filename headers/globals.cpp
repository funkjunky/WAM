#ifndef GLOBALS_CPP
#define GLOBALS_CPP

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

//Do not include globals.hpp... it's setup in such a way that including globals.hpp defines globals.cpp... so it comes after I think.
#include "structures.hpp"

namespace wam
{
	using namespace boost;
	typedef shared_ptr<Tag> TagPtr;

	//L0
	std::vector<shared_ptr<Tag> > HEAP;
		unsigned H=0, S=0; //Next term to be written, and next subterm to be written.
	std::vector<shared_ptr<Tag> > X;
	Mode MODE;
	bool FAIL = false;

	//L1
	std::vector<Instruction*> CODE; //I have no idea what this is for.
	unsigned P;	//next instrution to execute.
	std::map<Functor, unsigned> LABELS;
	//A registers? I don't get it.

	//L2
	std::vector<unsigned> STACK;
		unsigned E, CE, CP;
	std::vector<TagPtr> Y;

	//L3 
	unsigned num_of_args;
	unsigned B;
	unsigned HB;
	unsigned TR;
}
#endif

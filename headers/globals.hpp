//IMPORTANT: In order to use these globals, you must compile and include globals.o object from globals.cpp.
#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include "structures.hpp"

namespace wam
{
	using namespace boost;

	//convinient typedefs
	typedef boost::shared_ptr<Tag> TagPtr;
	typedef std::vector<boost::shared_ptr<Tag> >::iterator vtIter;
	//not used anywhere yet, but should be used everywhere their is an
	//address like for HEAP. or for the stack... maybe... I dunno...
	typedef unsigned Address;

	//L0
	extern std::vector<TagPtr> HEAP;
		extern unsigned H, S; //Next term to be written, and next subterm to be written.
	extern std::vector<TagPtr> X;
	extern Mode MODE;
	extern bool FAIL;

	//L1
	extern std::vector<InstructionPtr> CODE; //no idea.
	extern unsigned P;	//next instruction to execute.
	//This is my own... this is for call, to be able to set instructions.
	extern std::map<Functor, unsigned> LABELS;
	//A registers?

	//L2
	extern std::vector<unsigned> STACK;
		extern unsigned E, CE, CP;
	extern std::vector<TagPtr> Y;

	//L3 
	extern unsigned num_of_args;
	extern unsigned B;
	extern unsigned HB;
	extern unsigned TR;
}
#endif

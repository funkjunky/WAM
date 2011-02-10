#ifndef NOTWAMFUNCTIONS_CPP
#define NOTWAMFUNCTIONS_CPP

#include "notWAMFunctions.hpp"

#include <vector>
#include <sstream>

#include <boost/make_shared.hpp>

#include "structures.hpp"
#include "functions.hpp"
#include "globals.hpp"

namespace wam
{
	using std::vector;
	typedef vector<Fact>::iterator factIterator;
	//lineNum is necessary for the label.
	//lineNum, should represent the first line of the following set of instructions! IMPORTANT
	vector<InstructionPtr> getFunctorFactsInstructions(
					unsigned lineNum, factIterator begin, factIterator end)
	{
		using boost::make_shared;
		vector<InstructionPtr> instructions;
		vector<InstructionPtr> tempV;
		unsigned labelID = 0;
		
		LABELS[begin->getFunctor()] = lineNum;
		
		for(factIterator it = begin; it != end; ++it)
		{
			tempV = it->getInstructions();
			//This is in front, because if their is only one fact, then
			//we need to just trust it.
			if((it+1) == end)
			{
				instructions.push_back(make_shared<trust_me>());
				instructions.insert(instructions.end(), tempV.begin()
											, tempV.end());
			}
			else if(it == begin)
			{
				//the 2 is for the current label instruction and proceed.
				instructions.push_back(make_shared<try_me_else>(
												(lineNum += tempV.size() + 3)));
				instructions.insert(instructions.end(), tempV.begin()
											, tempV.end());
				instructions.push_back(make_shared<proceed>());
			}
			else
			{
				//the 2 is for the current label instruction and proceed.
				instructions.push_back(make_shared<retry_me_else>(
													(lineNum += tempV.size() + 2)));
				instructions.insert(instructions.end(), tempV.begin()
											, tempV.end());
				instructions.push_back(make_shared<proceed>());
			}
		}
		return instructions;
	}

	void PrologCleanUp()
	{
		for(std::vector<InstructionPtr>::iterator it = CODE.begin();
				it != CODE.end();
				++it)
			it->reset();

		CODE.clear();
		X.clear();
		Y.clear();
		STACK.clear();
		H = S = P = E = CE = CP = B = HB = TR = num_of_args = 0;
		LABELS.clear();
		FAIL = false;
	}

	void printHEAP()
	{
		using namespace std;
		for(vtIter i = HEAP.begin(); i != HEAP.end(); ++i)
		{
			const TagPtr I = *i;
			cout << "<" << IDtoString(I->Id()) << ", ";
			if(I->Id() == REF || I->Id() == STR)
				cout << I->Addr();
			else if(I->Id() == FNC)
				cout<< I->Functor()->name << "/" << I->Functor()->arity;
			else
				throw "id is invalid";
			cout << ">" << endl;
		}
	}

	std::string IDtoString(ID id)
	{
		switch(id)
		{
			case REF: return "REF";
			case STR: return "STR";
			case FNC: return "FNC";
			default: throw "invalid id";
		}
	}
}

#endif

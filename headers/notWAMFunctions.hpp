#ifndef NOTWAMFUNCTIONS_HPP
#define NOTWAMFUNCTIONS_HPP

#include <string>
#include <vector>

#include "structures.hpp"
#include "notWAMStructures.hpp"

namespace wam
{
	using std::vector;
	typedef vector<Fact>::iterator factIterator;

	vector<InstructionPtr> getFunctorFactsInstructions(
		unsigned lineNum, factIterator begin, factIterator end);

	void printHEAP();

	std::string IDtoString(ID id);

	void PrologCleanUp();
}

#endif

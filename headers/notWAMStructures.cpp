#ifndef NOTWAMSTRUCTURES_CPP
#define NOTWAMSTRUCTURES_CPP

#include "notWAMStructures.hpp"

#include "structures.hpp"
#include "functions.hpp"
#include "globals.hpp"

#include "vectorlock.hpp"

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

//TODO: remove this
#include <iostream>

using namespace boost;

namespace wam
{
	//Variable implementations
	const std::string& Variable::Name()
	{
		return name;
	}
	
	//Variable's ProducesInstruction Implementation
	std::string Variable::Id()
	{
		return name;
	}

	std::vector<InstructionPtr> Variable::getDefnInstruction
											(RegisterTool& regTool, TagPtr Ai
										,	VectorLock<InstructionPtr>& lingerInstrs)
	{
		using boost::make_shared;

		bool usedAlready = regTool.isVariableAssigned(name);
		TagPtr x = regTool.getVariable(Id()); 

		if(usedAlready)
			return std::vector<InstructionPtr>
						(1, make_shared<get_value>(x, Ai));
		else
			return std::vector<InstructionPtr>
						(1, make_shared<get_variable>(x, Ai));
	}

	std::vector<InstructionPtr> Variable::getInnerDefnInstruction
										(RegisterTool& regTool
											, VectorLock<InstructionPtr>& lingerInstrs)
	{
		using boost::make_shared;

		//uses unify_variable and unify_value
		bool usedAlready = regTool.isVariableAssigned(name);
		TagPtr x = regTool.getVariable(Id()); 

		if(usedAlready)
			return std::vector<InstructionPtr>
						(1, make_shared<unify_value>(x));
		else
			return std::vector<InstructionPtr>
						(1, make_shared<unify_variable>(x));
	}

	std::vector<InstructionPtr> Variable::getInnerClauseInstruction
											(RegisterTool& regTool
											, VectorLock<InstructionPtr>& lingerInstrs)
	{
		using boost::make_shared;

		bool usedAlready = regTool.isVariableAssigned(name);
		TagPtr x = regTool.getVariable(Id()); 

		if(usedAlready)
			return std::vector<InstructionPtr>
						(1, make_shared<set_value>(x));
		else
			return std::vector<InstructionPtr>
						(1, make_shared<set_variable>(x));
	}
	std::vector<InstructionPtr> Variable::getClauseInstruction
											(RegisterTool& regTool, TagPtr Ai
										,	VectorLock<InstructionPtr>& lingerInstrs)
	{
		using boost::make_shared;

		bool usedAlready = regTool.isVariableAssigned(name);
		TagPtr x = regTool.getVariable(Id()); 

		if(usedAlready)
			return std::vector<InstructionPtr>
						(1, make_shared<put_value>(x, Ai));
		else
			return std::vector<InstructionPtr>
						(1, make_shared<put_variable>(x, Ai));
	}


	Clause::Clause(Functor _structure
						, std::vector<ProducesInstructionPtr> _parameters)
		:structure(_structure), parameters(_parameters)
	{}

	const Functor& Clause::Structure()
	{
		return structure;
	}
	const std::vector<ProducesInstructionPtr>& Clause::Parameters()
	{
		return parameters;
	}

	std::string Clause::Id()
	{
		using std::ostringstream;
		ostringstream out; 
		out << Structure().name << "/" << Structure().arity;
		return out.str();
	}

	std::vector<InstructionPtr> Clause::getDefnInstruction
											(RegisterTool& regTool, TagPtr Ai
										,	VectorLock<InstructionPtr>& lingerInstrs)
	{
		using namespace std;
		using boost::make_shared;

		vector<InstructionPtr> instructions;

		instructions.push_back(
			boost::make_shared<get_structure>(make_shared<Functor>(structure)
																, Ai)); //A[ari]

		for(vector<ProducesInstructionPtr>::const_iterator it = parameters.begin()
				  ; it != parameters.end()
				  ; ++it)
		{
			vector<InstructionPtr> paramInstr 
					= (*it)->getInnerDefnInstruction(regTool, lingerInstrs);
			instructions.insert(instructions.end(), paramInstr.begin()
										, paramInstr.end());
		}

		return instructions;
	}

	std::vector<InstructionPtr> Clause::getInnerDefnInstruction
											(RegisterTool& regTool
											, VectorLock<InstructionPtr>& lingerInstrs)
	{
		using namespace std;
		using boost::make_shared;


		vector<InstructionPtr> instructions;
		vector<InstructionPtr> lingerInstr;

		bool usedAlready = regTool.isVariableAssigned(Id());
		//variable to be assigned to this strucure.
		TagPtr x = regTool.getVariable(Id()); 

		//see page 19... this is a bit akward.
		//We want to do set_structure, etc, but rather than using an
		//arguement register we use the unified variable register.

		//HERE
		VectorLock<ProducesInstructionPtr>::QueueIndex i 
				= lingerInstrs.reserve();
		lingerInstr = getDefnInstruction(regTool, x, lingerInstrs);
		lingerInstrs.append
				(i, lingerInstr.begin(), lingerInstr.end());

		if(usedAlready)
			return std::vector<InstructionPtr>
						(1, make_shared<unify_value>(x));
		else
			return std::vector<InstructionPtr>
						(1, make_shared<unify_variable>(x));
	}

	std::vector<InstructionPtr> Clause::getClauseInstruction
											(RegisterTool& regTool, TagPtr Ai
										,	VectorLock<InstructionPtr>& lingerInstrs)
	{		
		using namespace std;
		using boost::make_shared;

		vector<InstructionPtr> instructions;

		instructions.push_back(
			make_shared<put_structure>(make_shared<Functor>(structure), Ai));

		for(vector<ProducesInstructionPtr>::const_iterator it = parameters.begin()
				  ; it != parameters.end()
				  ; ++it)
		{
			vector<InstructionPtr> paramInstr 
					= (*it)->getInnerClauseInstruction(regTool, lingerInstrs);
			instructions.insert(instructions.end(), paramInstr.begin()
										, paramInstr.end());
		}

		return instructions;
	}


	std::vector<InstructionPtr> Clause::getInnerClauseInstruction
											(RegisterTool& regTool
											, VectorLock<InstructionPtr>& lingerInstrs)
	{		
		using namespace std;
		using boost::make_shared;


		vector<InstructionPtr> instructions;
		vector<InstructionPtr> lingerInstr;

		bool usedAlready = regTool.isVariableAssigned(Id());
		//variable to be assigned to this strucure.
		TagPtr x = regTool.getVariable(Id()); 

		//see page 19... this is a bit akward.
		//We want to do set_structure, etc, but rather than using an
		//arguement register we use the unified variable register.
		VectorLock<ProducesInstructionPtr>::QueueIndex i 
				= lingerInstrs.reserve();
		lingerInstr = getClauseInstruction(regTool, x, lingerInstrs);
		lingerInstrs.append
				(i, lingerInstr.begin(), lingerInstr.end());

		if(usedAlready)
			return std::vector<InstructionPtr>
						(1, make_shared<set_value>(x));
		else
			return std::vector<InstructionPtr>
						(1, make_shared<set_variable>(x));
	}

	std::vector<InstructionPtr> Clause::getQueryInstruction
										(RegisterTool& regTool
										, VectorLock<InstructionPtr>& lingerInstrs)
	{
			using boost::make_shared;

			std::vector<InstructionPtr> instructions;
			std::vector<InstructionPtr> tempInstr;

			regTool.resetRegIndices();
			//prepare arguement registers... should probablty be in its own
			//function...
			vector<TagPtr> A;
			for(vector<ProducesInstructionPtr>::const_iterator it2 
					  		= Parameters().begin();
					it2 != Parameters().end();
					++it2)
				A.push_back(regTool.getNextAvailableRegister(false));

			unsigned count = 0;
			for(vector<ProducesInstructionPtr>::const_iterator it2 
					  		= Parameters().begin();
					it2 != Parameters().end();
					++it2)
			{
				tempInstr = (*it2)->getClauseInstruction
									(regTool, A[count], lingerInstrs);
				++count;
				instructions.insert
					(instructions.end(), tempInstr.begin(), tempInstr.end());
			}
			//call f/#
			instructions.push_back(make_shared<call>(
											make_shared<Functor>(Structure())));

			return instructions;
	}

	

	Fact::Fact(const Clause& _defn)
		:defn(_defn), goals()
	{}

	Fact::Fact(const Clause& _defn, const std::vector<Clause>& _goals)
		:defn(_defn), goals(_goals)
	{}

	Clause Fact::Defn() const
	{
		return defn;
	}

	std::vector<Clause> Fact::Goals() const
	{
		return goals;
	}

	void Fact::addClause(Clause _clause)
	{
		goals.push_back(_clause);
	}

	Functor Fact::getFunctor()
	{
		return defn.Structure();
	}

	std::vector<InstructionPtr> Fact::getInstructions()
	{
		using namespace std;
		using boost::make_shared;
		RegisterTool regTool;
		vector<InstructionPtr> instructions;
		vector<InstructionPtr> tempInstr;
		VectorLock<InstructionPtr> lingeringStrInstructions;

		unsigned numOfPermenants = regTool.setAndGetNumOfPermenants(*this);

		if(numOfPermenants > 0)
			instructions.push_back(
				make_shared<allocate>(numOfPermenants));

		regTool.resetRegIndices();
		//prepare Arguement registers for the clause.
		vector<TagPtr> A;
		for(vector<ProducesInstructionPtr>::const_iterator it 
				  		= defn.Parameters().begin()
				  ; it != defn.Parameters().end()
				  ; ++it)
			A.push_back(regTool.getNextAvailableRegister(false));

		unsigned count = 0;
		for(vector<ProducesInstructionPtr>::const_iterator it 
				  		= defn.Parameters().begin()
				  ; it != defn.Parameters().end()
				  ; ++it)
		{
			tempInstr = (*it)->getDefnInstruction
								(regTool, A[count], lingeringStrInstructions);
			instructions.insert
				(instructions.end(), tempInstr.begin(), tempInstr.end());
			++count;
		}


		for(vector<Clause>::iterator it = goals.begin()
				  ; it != goals.end()
				  ; ++it)
		{
			tempInstr = it->getQueryInstruction(regTool, lingeringStrInstructions);
			instructions.insert
				(instructions.end(), tempInstr.begin(), tempInstr.end());
		}

		//append the lingering instructions from structures.
		//Then clear the lingering instructions so this can be called again,
		//without side effect.
		instructions.insert(instructions.end(), 
									lingeringStrInstructions.getVector().begin(), 
									lingeringStrInstructions.getVector().end());
		lingeringStrInstructions.clear();

		if(numOfPermenants > 0)
			instructions.push_back(make_shared<deallocate>());

		return instructions;
	}

	//REGISTER TOOLS

	unsigned RegisterTool::setAndGetNumOfPermenants(const Fact& fact)
	{
		using namespace std;

		if(fact.goals.size() == 0)
			return 0;

		unsigned numOfPerms = 0;

		vector<ProducesInstructionPtr> firstSet(fact.Defn().Parameters());
		firstSet.insert(firstSet.end(), fact.Goals()[0].Parameters().begin(),
								fact.Goals()[0].Parameters().end());

		for(vector<ProducesInstructionPtr>::const_iterator it = firstSet.begin()
				  ; it != firstSet.end()
				  ; ++it)
			variableCount[(*it)->Id()] = 1;

		{
			vector<string> clauseParamIDs;
			for(unsigned i=1; i != fact.Goals().size(); ++i)
			{
				for(vector<ProducesInstructionPtr>::const_iterator it 
						  		= fact.Goals()[i].Parameters().begin()
							; it != fact.Goals()[i].Parameters().end()
							; ++it)
			  		clauseParamIDs.push_back((*it)->Id());

				//parameters in DIFFERENT goals... therefore we only want to
				//count each parameter once.
				unique(clauseParamIDs.begin(), clauseParamIDs.end());
				for(vector<string>::const_iterator it = clauseParamIDs.begin()
						  ; it != clauseParamIDs.end()
						  ; ++it)
					if(++variableCount[*it] == 2)
						++numOfPerms;
			}
		}

		return numOfPerms;
	}


	bool RegisterTool::isVariableAssigned(const std::string& name)
	{
		return varNameRefs.find(name) != varNameRefs.end();
	}

	TagPtr RegisterTool::getVariable(const std::string& name)
	{
		if(!isVariableAssigned(name))
			varNameRefs[name] = getNextAvailableRegister(isPermenant(name));

		return varNameRefs[name];
	}

	bool RegisterTool::isPermenant(const string& name)
	{
		//this means their were no goals in the fact.
		if(variableCount.size() == 0)
			return false;

		return variableCount[name] >= 2;
	}

	TagPtr RegisterTool::getNextAvailableRegister(bool isPermenant)
	{
		using boost::make_shared;

		if(isPermenant)
		{
			ensureSpace(Y, pIndex+1);
			//If we haven't assigned anything to the tag yet... then put a
			//place holder in (hence PH)
			if(Y[pIndex] == TagPtr())
				Y[pIndex] = make_shared<Tag>(PH, pIndex);

			//we increment after grabbing the latests index
			return Y[pIndex++];
		}
		else
		{
			ensureSpace(X, tIndex+1);
			//If we haven't assigned anything to the tag yet... then put a
			//place holder in (hence PH)
			if(X[tIndex] == TagPtr())
				X[tIndex] = make_shared<Tag>(PH, tIndex);

			//we increment after grabbing the latests index
			return X[tIndex++];
		}
	}
	void RegisterTool::resetRegIndices()
	{
		pIndex = 0;
		tIndex = 0;
	}


}
#endif

#ifndef NOTWAMSTRUCTURES_HPP
#define NOTWAMSTRUCTURES_HPP

#include "structures.hpp"
#include "vectorlock.hpp"

#include <vector>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

namespace wam
{
	class ProducesInstruction;
	class Fact;
	class Clause;
	class Variable;
	class RegisterTool;

	//TODO: ProducesInstructions with an s at the end!
	class ProducesInstruction
	{
		public:
		virtual std::vector<InstructionPtr> getDefnInstruction
								(RegisterTool& regTool, TagPtr Ai
									, VectorLock<InstructionPtr>& lingerInstrs) = 0;
		virtual std::vector<InstructionPtr> getInnerDefnInstruction
								(RegisterTool& regTool
									, VectorLock<InstructionPtr>& lingerInstrs) = 0;
		virtual std::vector<InstructionPtr> getClauseInstruction
								(RegisterTool& regTool, TagPtr Ai
									, VectorLock<InstructionPtr>& lingerInstrs) = 0;
		virtual std::vector<InstructionPtr> getInnerClauseInstruction
								(RegisterTool& regTool
									, VectorLock<InstructionPtr>& lingerInstrs) = 0;
		virtual std::string Id() = 0;
	};
	typedef boost::shared_ptr<ProducesInstruction> ProducesInstructionPtr;

	class Clause : public ProducesInstruction //IsImplementationOf
	{
		private:
		Clause();

		Functor structure;
		std::vector<ProducesInstructionPtr> parameters;

		public:
		Clause(Functor _structure
					, std::vector<ProducesInstructionPtr> _parameters);

		const Functor& Structure();
		const std::vector<ProducesInstructionPtr>& Parameters();

		//necessary to make Clause NOT abstract.
		virtual std::string Id();
		virtual std::vector<InstructionPtr> getDefnInstruction
								(RegisterTool& regTool, TagPtr Ai
									, VectorLock<InstructionPtr>& lingerInstrs);
		virtual std::vector<InstructionPtr> getInnerDefnInstruction
								(RegisterTool& regTool
									, VectorLock<InstructionPtr>& lingerInstrs);
		virtual std::vector<InstructionPtr> getClauseInstruction
								(RegisterTool& regTool, TagPtr Ai
									, VectorLock<InstructionPtr>& lingerInstrs);
		virtual std::vector<InstructionPtr> getInnerClauseInstruction
								(RegisterTool& regTool
									, VectorLock<InstructionPtr>& lingerInstrs);

		//An extra instruction getter just for Clauses!
		std::vector<InstructionPtr> getQueryInstruction(RegisterTool& regTool
										, VectorLock<InstructionPtr>& lingerInstrs);
	};

	class Variable : public ProducesInstruction //IsImplementationOf
	{
		private:
		Variable();

		std::string name;
		unsigned ari; //arguementRegisterIndex

		public:
		//ya I defined it here... big whoop, wanna fight about it?
		Variable(const std::string& _name, unsigned _ari = 0)
			:name(_name), ari(_ari)
		{}
		const std::string& Name();

		//necessary to make Clause NOT abstract.
		virtual std::string Id();
		virtual std::vector<InstructionPtr> getDefnInstruction
								(RegisterTool& regTool, TagPtr Ai
									, VectorLock<InstructionPtr>& lingerInstrs);
		virtual std::vector<InstructionPtr> getInnerDefnInstruction
								(RegisterTool& regTool
									, VectorLock<InstructionPtr>& lingerInstrs);
		virtual std::vector<InstructionPtr> getClauseInstruction
								(RegisterTool& regTool, TagPtr Ai
									, VectorLock<InstructionPtr>& lingerInstrs);
		virtual std::vector<InstructionPtr> getInnerClauseInstruction
								(RegisterTool& regTool
									, VectorLock<InstructionPtr>& lingerInstrs);
	};

	class RegisterTool
	{
		private:
		std::map<std::string, TagPtr> varNameRefs;

		std::map<std::string, unsigned> variableCount;

		//for getNextAvailableRegister
		unsigned pIndex;
		unsigned tIndex;

		bool isPermenant(const std::string& name);
	
		public:
		//isVariableAssigned must be called before getVariable. Obviously.
		bool isVariableAssigned(const std::string& name);
		TagPtr getVariable(const std::string& name);
		void resetRegIndices();
		TagPtr getNextAvailableRegister(bool isPermenant);

		//TODO: fix wrong spelling of Permanent
		unsigned setAndGetNumOfPermenants(const Fact& fact);
	};

	class Fact
	{
		private:
		Fact();

		Clause defn;
		std::vector<Clause> goals;

		public:
		Fact(const Clause& _defn);
		Fact(const Clause& _defn, const std::vector<Clause>& _goals);

		Clause Defn() const;
		std::vector<Clause> Goals() const;

		//ya I'll allow one public variable
		VectorLock<InstructionPtr> lingeringStrInstructions;

		void addClause(Clause _clause);

		Functor getFunctor();

		std::vector<InstructionPtr> getInstructions();
		friend unsigned RegisterTool::setAndGetNumOfPermenants
								(const Fact& fact);
	};
}

#endif

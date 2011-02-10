#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

//What's in this file?
//enums:
//	ID
//	Mode
//structs:
//	Functor
//	Value
//	Tag
// Instruction
// ProducesInstructions

#include <string>
#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

namespace wam
{
	using namespace boost;

	enum ID
	{
		REF, STR, FNC, PH
	};

	struct Functor
	{
		Functor()
			:name(""), arity(0)
		{}
		Functor(std::string _name, unsigned _arity)
			:name(_name), arity(_arity)
		{}

		std::string name;
		unsigned arity;

		operator std::string()
		{
			std::ostringstream out;
			out << name << "/" << arity;
			return out.str();
		}
	};
	typedef shared_ptr<Functor> FunctorPtr;
	//for now it will be member by member comparison... perhaps later I'll decide pointer comparison is in order.
	inline bool operator==(const Functor& f1, const Functor& f2)
	{
		return f1.name == f2.name && f1.arity == f2.arity;
	}
	inline bool operator<(const Functor& f1, const Functor& f2)
	{
		return f1.name < f2.name 
					|| (f1.name == f2.name && f1.arity < f2.arity);
	}

	class Tag
	{
		class ValueVisitor;

		public:
		//This constructor isn't very good. It's the same as an unbound variable in the first HEAP slot.
		//TODO: some how set functor to NULL, or use the boost::any thing... this is dangerous.
		Tag()
			:id(REF), value()
		{}
		Tag(ID _id, FunctorPtr _functor)
			:id(_id), value(_functor)
		{
			//std::cerr << "id: " << id << std::endl;
			//std::cerr << "start Tag constructor..." << std::endl;
		}
		Tag(ID _id, unsigned _addr)
			:id(_id), value(_addr)
		{
		}

		//confusing? Ya just a tad eh?
		ID Id() const {return id;}
		unsigned Addr() {
			return apply_visitor(addrVisitor(), value);
		}
		FunctorPtr Functor() {
			return apply_visitor(functorVisitor(), value);
		}

		~Tag()
		{}

/*
		class functorOrNullVisitor 
			: public static_visitor<const FunctorPtr&> {
			public:
				const FunctorPtr& operator() (unsigned & addr) const
				{ return FunctorPtr(); }

				const FunctorPtr& operator() (const FunctorPtr& functor) const
				{ return functor; }
		};
		*/
		private:
		ID id;
		//TODO: figure out how to make functor a pointer again.
		//Note: check out boost's "Any" class.
		//union
		//{
			//unsigned: addr
			//FunctorPtr: functor
			variant<unsigned, FunctorPtr> value;
		//};

		class addrVisitor : public static_visitor<unsigned> {
			public:
				unsigned operator() (unsigned & addr) const
				{ return addr; }

				unsigned operator() (FunctorPtr functor) const
				{ throw "accessing addr, when functor is current variant."; }
		};

		class functorVisitor : public static_visitor<FunctorPtr> {
			public:
				FunctorPtr operator() (unsigned & addr) const
				{ throw "accessing a functorPtr, when unsigned is current variant."; }

				FunctorPtr operator() (FunctorPtr functor) const
				{ return functor; }
		};
	};

	typedef shared_ptr<Tag> TagPtr;

	enum Mode
	{
		READ, WRITE
	};

	//All instructions are classes inheriting from Instruction.
	//Their parameters are member variables of their class.
	//The only the the instruction classes require are the call method.
	//The call method always returns void and takes no arguements.
	//To only call the instruction without storing it or anything,
	//just use the cast operator and the call method.
	//Ie. put_structure(fx, reg).trigger();
	class Instruction
	{
		public:
		virtual void trigger() = 0;
		virtual operator std::string() = 0;
	};
	typedef shared_ptr<Instruction> InstructionPtr;
}
#endif

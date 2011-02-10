#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include "structures.hpp"

//globals to work with: HEAP, X, MODE
namespace wam
{
	//L0
	//use copy constructor to copy functor into place.
	//put_structure passes by value for fx... this is because we want to use temporaries.
	class put_structure : public Instruction
	{
		private:
		put_structure(); //default constructor not allowed!

		FunctorPtr fx;
		TagPtr reg;

		public:
		//reg: register reference. Ie. X[1]
		put_structure(FunctorPtr _fx, TagPtr _reg)
			:fx(_fx), reg(_reg)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class set_variable : public Instruction
	{
		private:
		set_variable(); //default constructor not allowed!

		TagPtr reg;

		public:
		//reg: register reference. Ie. X[1]
		set_variable(TagPtr _reg)
			:reg(_reg)
		{}

		virtual void trigger();
		virtual operator std::string();
	};
	class set_value : public Instruction
	{
		private:
		set_value();

		const TagPtr reg;

		public:
		//reg: register reference. Ie. X[1]
		set_value(const TagPtr _reg)
			:reg(_reg)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class get_structure : public Instruction
	{
		private:
		get_structure();

		const FunctorPtr fx;
		const TagPtr reg;

		public:
		get_structure(const FunctorPtr &_fx, const TagPtr _reg)
			:fx(_fx), reg(_reg)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class unify_variable : public Instruction
	{
		private:
		unify_variable();

		TagPtr reg;

		public:
			 unify_variable(TagPtr _reg)
			  	:reg(_reg)
			{}

		virtual void trigger();
		virtual operator std::string();
	};

	class unify_value : public Instruction
	{
		private:
		unify_value();

		const TagPtr reg;

		public:
			unify_value(const TagPtr _reg)
				:reg(_reg)
			{}

		virtual void trigger();
		virtual operator std::string();
	};

	unsigned deref(TagPtr tag);
	void unify(TagPtr tag1, TagPtr tag2);
	void bind(unsigned addr1, unsigned addr2);
	bool unboundReference(unsigned addr);

	//L1

	//I guess the instruction size of addr for CODE.
	//I may modify this to just be a const TagPtr&, so I can get the instruction size for anything.
	unsigned instruction_size(unsigned addr);	//I have no idea...
	class call : public Instruction
	{
		private:
		call();

		const FunctorPtr functor;

		public:
		call(FunctorPtr _functor)
			:functor(_functor)
		{}
		
		virtual void trigger();
		virtual operator std::string();
	};
		bool _tagsFunctorMatch(TagPtr tag, FunctorPtr functor);
	class proceed : public Instruction
	{
		public:
		virtual void trigger();
		virtual operator std::string();
	};

	//put_variable(register, arguementRegister)
	class put_variable : public Instruction
	{
		private:
		put_variable(); //default constructor not allowed!

		TagPtr reg1;
		TagPtr reg2;

		public:
		//reg: register reference. Ie. X[1]
		put_variable(TagPtr _reg1, TagPtr _reg2)
			:reg1(_reg1), reg2(_reg2)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class put_value : public Instruction
	{
		private:
		put_value(); //default constructor not allowed!

		TagPtr reg1;
		TagPtr reg2;

		public:
		//reg: register reference. Ie. X[1]
		put_value(TagPtr _reg1, TagPtr _reg2)
			:reg1(_reg1), reg2(_reg2)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class get_variable : public Instruction
	{
		private:
		get_variable(); //default constructor not allowed!

		TagPtr reg1;
		TagPtr reg2;

		public:
		//reg: register reference. Ie. X[1]
		get_variable(TagPtr _reg1, TagPtr _reg2)
			:reg1(_reg1), reg2(_reg2)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class get_value : public Instruction
	{
		private:
		get_value(); //default constructor not allowed!

		TagPtr reg1;
		TagPtr reg2;

		public:
		//reg: register reference. Ie. X[1]
		get_value(TagPtr _reg1, TagPtr _reg2)
			:reg1(_reg1), reg2(_reg2)
		{}

		virtual void trigger();
		virtual operator std::string();
	};


	//L2
	class allocate : public Instruction
	{
		private:
		allocate();

		unsigned N;

		public:
		allocate(unsigned _N)
			:N(_N)
		{}

		virtual void trigger();
		virtual operator std::string();
	};

	class deallocate : public Instruction
	{
		public:
		virtual void trigger();
		virtual operator std::string();
	};
	//L3	
	void backtrack();
	void unwind_trail(unsigned a1, unsigned a2);

	class try_me_else : public Instruction
	{
		private:
		try_me_else();

		unsigned L;

		public:
		try_me_else(unsigned _L)
			:L(_L)
		{}

		virtual void trigger();
		virtual operator std::string();
	};
	class retry_me_else : public Instruction
	{
		private:
		retry_me_else();

		unsigned L;

		public:
		retry_me_else(unsigned _L)
			:L(_L)
		{}

		virtual void trigger();
		virtual operator std::string();
	};
	class trust_me : public Instruction
	{
		public:
		virtual void trigger();
		virtual operator std::string();
	};

	//My functions.
	void ensureHEAP(unsigned maxIndex);
	void ensureSTACK(unsigned maxIndex);
	template <typename T>
	void ensureSpace(T& coll, unsigned maxIndex)
	{
		if(coll.size() <= maxIndex)
			coll.resize(maxIndex+1);
	}
}
#endif

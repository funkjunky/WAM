#include <stack>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/make_shared.hpp>

#include "functions.hpp"
#include "structures.hpp"
#include "globals.hpp"

//TODO: remove this
#include <iostream>

namespace wam
{
	using namespace boost;

	//forward declaration... only used here.
	std::string convertToString(TagPtr x);

	//Parameters:
	//FunctorPtr fx
	//TagPtr reg
	void put_structure::trigger()
	{
		ensureHEAP(H+1);
		HEAP[H]		=	make_shared<Tag>(STR, H+1);
		HEAP[H+1]	=	make_shared<Tag>(FNC, fx);
		(*reg)			=	*(HEAP[H]);
		H += 2;
	}
	 
	put_structure::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "put_structure(" << string(*fx) << ", " << convertToString(reg) << ")";
		return out.str();
	}


	//Parameters:
	//TagPtr reg
	void set_variable::trigger()
	{
		ensureHEAP(H);
		HEAP[H]		=	make_shared<Tag>(REF, H);
		(*reg)			=	*(HEAP[H]);
		H += 1;
	}
	set_variable::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "set_variable(" << convertToString(reg) << ")";
		return out.str();
	}

	//Parameters:
	//TagPtr reg
	void set_value::trigger()
	{
		ensureHEAP(H);
		(*(HEAP[H]))		= *reg;	//use copy constructor, in case reg changes.
		H += 1;
	}
	set_value::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "set_variable(" << convertToString(reg) << ")";
		return out.str();
	}

	//Parameters:
	//FunctorPtr fx
	//TagPtr reg
	void get_structure::trigger()
	{
		ensureHEAP(H+1);
		unsigned addr = deref(reg);
		//The book says "STORE", which includes all data structures.
		switch(HEAP[addr]->Id())
		{
			case REF :
				HEAP[H]		= 	make_shared<Tag>(STR, H+1);
				//TODO: Eventually I'll make the Functor in Tag a const, and then I won't need to deref the FunctorPtr, in fact it'll be safer if I don't and I'll just copy construct it over as a const.
				HEAP[H+1]	= 	make_shared<Tag>(FNC, make_shared<Functor>(*fx));
				bind(addr, H);
				H += 2;
				MODE = WRITE;
				break;
			case STR :
			{
				//what i wanted: typedef HEAP[addr].value.addr a;
				const unsigned a = HEAP[addr]->Addr();
				if((*(HEAP[a]->Functor())) == *fx)
				{
					S = a + 1;
					MODE = READ;
				}
				else
					FAIL = true;
			}
				break;
			default :
				FAIL = true;
		}
	}
	get_structure::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "get_structure(" << string(*fx) << ", " << convertToString(reg) << ")";
		return out.str();
	}

	//Parameters:
	//TagPtr reg
	void unify_variable::trigger()
	{
		ensureHEAP(H);
		switch(MODE)
		{
			case READ:
				(*reg)		= *(HEAP[S]);
				break;
			case WRITE:
				HEAP[H]	= make_shared<Tag>(REF, H);
				(*reg)		= *(HEAP[H]);
				H += 1;
				break;
		}
		S += 1;
	}
	unify_variable::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "set_variable(" << convertToString(reg) << ")";
		return out.str();
	}

	//Parameters:
	//TagPtr reg
	void unify_value::trigger()
	{
		switch(MODE)
		{
			case READ:
				unify(reg, HEAP[S]);	//unify(reg, S)
				break;
			case WRITE:
				*(HEAP[H]) = *reg;
				H += 1;
				break;
		}
		S += 1;
	}
	unify_value::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "unify_value(" << convertToString(reg) << ")";
		return out.str();
	}

	//it's passed a Tag, this way registers can pass to it, and still have their value dereferenced.
	unsigned deref(TagPtr initTag)
	{
		const unsigned a = initTag->Addr();
		//again they use STORE here... hmmmm... grrr... how does it work.
		const unsigned tag = HEAP[a]->Id();
		const unsigned value = HEAP[a]->Addr();

		if(tag == REF || value != a)
			return deref(HEAP[a]);
		else
			return a;
	}

	//IGNORING STORE! Stupid store... how the hell does it work...
	//using Tag instead of addresses, to make things easier.
	//It's hard to explain.
	std::stack<TagPtr> PDL;
	void unify(TagPtr tag1, TagPtr tag2)
	{
		using namespace std;
		PDL.push(tag1); PDL.push(tag2);
		FAIL = false;	//hmm is this necessary
		unsigned d1, d2;
		ID t1, t2;
		Functor f1, f2;
		while(!PDL.empty() || FAIL)
		{
			d1 = deref(PDL.top()); PDL.pop();
			d2 = deref(PDL.top()); PDL.pop();
			if(d1 != d2)
			{
				t1 = HEAP[d1]->Id(); t2 = HEAP[d2]->Id();
				if(t1 == REF || t2 == REF)
					bind(d1, d2);
				else
				{
					if(HEAP[d1]->Id() == STR || HEAP[d2]->Id() == STR)
						throw "we may have a problem... pg15 no STR case";
					f1 = *(HEAP[d1]->Functor()); f2 = *(HEAP[d2]->Functor());
					//TODO: this should probably just be f1 == f2, and let the equality operator take care of the rest.
					if(f1.name == f2.name && f1.arity == f2.arity)
					{
						for(int i=0; i!=f1.arity; ++i)
						{
							PDL.push(HEAP[d1 + i]);
							PDL.push(HEAP[d2 + i]);
						}
					}
					else
						FAIL = true;
				}
			}
		}
	}
	
	//pg14[HASSAN] states if they are both unbound arbitrarily choose.
	void bind(unsigned addr1, unsigned addr2)
	{
		if(unboundReference(addr1) && !unboundReference(addr2))
			(*(HEAP[addr1])) = *(HEAP[addr2]);	
		else if(!unboundReference(addr1) && unboundReference(addr2))
			(*(HEAP[addr2])) = *(HEAP[addr1]);
		else if(!unboundReference(addr1) && !unboundReference(addr2))
			throw "you can't bind 2 unbound references!";
		else
			(*(HEAP[addr1])) = *(HEAP[addr2]);
	}
	bool unboundReference(unsigned addr)
	{
		return HEAP[addr]->Id() == REF && HEAP[addr]->Addr() == addr;
	}

	//L1

	//in a more low level compiler this would be necessary...
	//in my case I'm use a vector... so every instruction is exactly
	//1 index after the one before it.
	unsigned instruction_size(unsigned addr)
	{
		return 1;
	}

	void call::trigger()
	{
		if(LABELS.find(*functor) == LABELS.end());
			throw "functor doesn't exist yet... that could be an issue.";

		CP = P + instruction_size(P);
		num_of_args = functor->arity;
		P = LABELS[*functor];
	}
	call::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "call(" << string(*functor) << ")";
		return out.str();
	}

	void proceed::trigger()
	{
		P = CP;
	}
	proceed::operator std::string()
	{
		return "proceed()";
	}

	void put_variable::trigger()
	{
		HEAP[H] = make_shared<Tag>(REF, H);
		*(reg1) = *(HEAP[H]);
		*(reg2) = *(HEAP[H]);
		++H;
	}
	put_variable::operator std::string()
	{
		using std::ostringstream;
		using std::string;
		ostringstream out;
		out << "put_variable(" << convertToString(reg1) << ", " 
				<< convertToString(reg2) << ")";
		return out.str();
	}

	void put_value::trigger()
	{
		*(reg2) = *reg1;
	}
	put_value::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "put_value(" << convertToString(reg1) << ", " 
				<< convertToString(reg2) << ")";
		return out.str();
	}

	void get_variable::trigger()
	{
		(*reg1) = *reg2;
	}
	get_variable::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "get_variable(" << convertToString(reg1) << ", " 
				<< convertToString(reg2) << ")";
		return out.str();
	}

	void get_value::trigger()
	{
		unify(reg1, reg2);
	}
	get_value::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "get_value(" << convertToString(reg1) << ", " 
				<< convertToString(reg2) << ")";
		return out.str();
	}

	//L2
	void allocate::trigger()
	{
		//TODO: leave explanation here for this... perhaps clean up.
		unsigned newE = E + STACK[E + 2] + 3;
		//ensure the stack is resized if necessary.
		ensureSTACK(newE + 2);

		STACK[newE] = E;
		STACK[newE + 1] = CP;
		STACK[newE + 2] = N;
		E = newE;
		P = P + instruction_size(P);
	}
	allocate::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "allocate(" << N << ")";
		return out.str();
	}

	void deallocate::trigger()
	{
		P = STACK[E + 1];
		E = STACK[E];
	}
	deallocate::operator std::string()
	{
		return "deallocate()";
	}

	//L3
	void backtrack()
	{
		P = STACK[B + STACK[B] + 4];
	}

	void unwind_trail(unsigned a1, unsigned a2)
	{
		throw "hmmm...";
		//TODO: figure out what to do here... STORE? Hmmm...
		//Check to see if they use STORE for everything, and ditch HEAP
		//and other things.
		//for(unsigned i = a1; i != a2; ++i)
		//	STORE[TRAIL[i]] = make_shared<Tag>(REF, TRAIL[i]);
	}

	void try_me_else::trigger()
	{
		unsigned newB;
		//TODO: fix the 3 and 7... maybe make a function that returns 
		//the correct address.
		if(E > B)
			newB = E + STACK[E + 2] + 3;
		else
			newB = B + STACK[B] + 7;

		STACK[newB] = num_of_args;
		unsigned n = STACK[newB];
		//make sure the STACK is resized as necessary, if necessary.
		ensureSTACK(B + n + 6);
		for(unsigned i = 1; i <= n; ++i)
			STACK[newB + i] = X[i]->Addr(); //really A[i]

		STACK[newB + n + 1] = E;
		STACK[newB + n + 2] = CP;
		STACK[newB + n + 3] = B;
		STACK[newB + n + 4] = L;
		STACK[newB + n + 5] = TR;
		STACK[newB + n + 6] = H;

		B = newB;
		HB = H;
		P = P + instruction_size(P);
	}
	try_me_else::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "try_me_else(" << L << ")";
		return out.str();
	}

	void retry_me_else::trigger()
	{
		unsigned n = STACK[B];
		//make sure the STACK is resized as necessary, if necessary.
		ensureSTACK(B + n + 4);
		for(unsigned i = 1; i <= n; ++i)
			*(X[i]) = Tag(REF, STACK[B + i]); //really A[i]

		E = STACK[B + n + 1];
		CP = STACK[B + n + 2];
		STACK[B + n + 4] = L;
		unwind_trail(STACK[B + n + 5], TR);
		TR = STACK[B + n + 5];
		H = STACK[B + n + 6];
		HB = H;
		P = P + instruction_size(P);
	}
	retry_me_else::operator std::string()
	{
		using std::string;
		using std::ostringstream;
		ostringstream out;
		out << "retry_me_else(" << L << ")";
		return out.str();
	}

	void trust_me::trigger()
	{
		unsigned n = STACK[B];
		for(unsigned i = 1; i <= n; ++i)
			*(X[i]) = Tag(REF, STACK[B + i]);	//really A[i]

		E = STACK[B + n + 1];
		CP = STACK[B + n + 2];
		unwind_trail(STACK[B + n + 5], TR);
		TR = STACK[B + n + 5];
		H = STACK[B + n + 6];
		B = STACK[B + n + 3];
		HB = STACK[B + n + 6];
		P = P + instruction_size(P);
	}
	trust_me::operator std::string()
	{
		using namespace std;
		return "trust_me()";
	}

	//Other functions
	void ensureHEAP(unsigned maxIndex)
	{
		if(HEAP.size() <= maxIndex)
			HEAP.resize(maxIndex+1);
	}

	void ensureSTACK(unsigned maxIndex)
	{
		if(STACK.size() <= maxIndex)
			STACK.resize(maxIndex+1);
	}

	//this is only used here! Hence no definition in functions.hpp
	std::string convertToString(TagPtr x)
	{
		std::ostringstream out;
		for(int i=0; i != X.size(); ++i)
			if(X[i] == x)
				out << "X[" << i << "]";
		for(int i=0; i != Y.size(); ++i)
			if(Y[i] == x)
				out << "Y[" << i << "]";

		if(out.str() == "")
			out << "???";

		return out.str();
	}
}

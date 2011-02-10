#include <iostream>
#include <vector>

#include <boost/make_shared.hpp>

#include <wam.hpp>

using namespace wam;
using namespace boost;

void L0Test();
void L0CODETest();
void printAllInstructions();
void simpleFacts();
void moderateFacts();
void queryForModerateFacts();

int main()
{
	//L0Test();
	//L0CODETest();

	cout << "first ensure the isntructions print out correctly:" << endl;
	printAllInstructions();
	cout << endl;

	cout << "next test moderateily difficult facts:" << endl;
	moderateFacts();
	queryForModerateFacts();
	cout << endl;

	PrologCleanUp();
}

void printAllInstructions()
{
	using std::cerr;
	using std::endl;
	using boost::make_shared;
	X.resize(2);
	cerr << string(put_structure(make_shared<Functor>("f",2), X[0])) << endl;
}

void simpleFacts()
{
	using boost::make_shared;
	using std::cout;
	using std::endl;

	PrologCleanUp();

	Functor defn_f1 = Functor("f", 1);
	Fact f1_1(Clause(defn_f1, vector<ProducesInstructionPtr>(1
									, make_shared<Variable>("X"))));
	vector<InstructionPtr> factsInstr = f1_1.getInstructions();

	cout << "Instructions for fact f(X).:" << endl;
	unsigned lineNum11 = 0;
	for(vector<InstructionPtr>::iterator it = factsInstr.begin()
				; it != factsInstr.end()
				; ++it)
		cout << (++lineNum11) << ": " << string(**it) << endl;
	cout << endl;

	//
	vector<InstructionPtr> instructions;
	vector<InstructionPtr> tmp;
	vector<Fact> tmpf;

	Functor defn_h2 = Functor("h", 2);
	Fact h2_1(Clause(defn_h2, vector<ProducesInstructionPtr>({
									make_shared<Variable>("X")
									, make_shared<Variable>("Y")})));

	tmpf.push_back(h2_1);
	tmp = getFunctorFactsInstructions(0
	, tmpf.begin(), tmpf.end());

	instructions.insert(instructions.end(), tmp.begin(), tmp.end());
	tmpf.clear();
								

	Functor defn_g3 = Functor("g", 3);
	Fact g3_1(Clause(defn_g3, vector<ProducesInstructionPtr>({
									make_shared<Variable>("X")
									, make_shared<Variable>("Y")
									, make_shared<Variable>("X")})));
	tmpf.push_back(g3_1);
	tmp = getFunctorFactsInstructions(instructions.size()
													, tmpf.begin(), tmpf.end());
	instructions.insert(instructions.end(), tmp.begin(), tmp.end());

	cout << "Instructions for facts h(X, Y). g(X, Y, X).:" << endl;
	unsigned lineNum = 0;
	for(vector<InstructionPtr>::iterator it = instructions.begin()
				; it != instructions.end()
				; ++it)
		cout << (++lineNum) << ": " << string(**it) << endl;
	cout << endl;
}

void moderateFacts()
{
	using boost::make_shared;
	using std::cout;
	using std::endl;
	using std::vector;

	PrologCleanUp();
	vector<ProducesInstructionPtr> nullVector;

	//example pg36
	//FYI this fact should always be true.
	Functor defn = Functor("p", 2);
	//p(X, a).
	Fact p2_1(Clause(defn, vector<ProducesInstructionPtr>({
			  						boost::make_shared<Variable>("X")
									, boost::make_shared<Clause>(Functor("a", 0)
										, nullVector)})));
	
	//p(b, X).
	Fact p2_2(Clause(defn, vector<ProducesInstructionPtr>({
									boost::make_shared<Clause>(Functor("b", 0)
										, nullVector)
									, boost::make_shared<Variable>("X")})));

	//p(X, Y) :- p(X, a), p(b, Y).
	//p(X, Y)
	Fact p2_3(Clause(defn, vector<ProducesInstructionPtr>({
									boost::make_shared<Variable>("X")
									, boost::make_shared<Variable>("Y")})));
	//:- p(X, a)
	p2_3.addClause(Clause(defn, vector<ProducesInstructionPtr>({
									boost::make_shared<Variable>("X")
									, boost::make_shared<Clause>(Functor("a", 0)
										, nullVector)})));
	//, p(b, Y).
	p2_3.addClause(Clause(defn, vector<ProducesInstructionPtr>({
									boost::make_shared<Clause>(Functor("b", 0)
										, nullVector)
									, boost::make_shared<Variable>("Y")})));

	vector<Fact> p2({p2_1, p2_2, p2_3});
	vector<InstructionPtr> instructions(
		getFunctorFactsInstructions(0, p2.begin(), p2.end()));

	cout << "Instructions for facts from pg 36:" << endl;
	unsigned lineNum = 0;
	for(vector<InstructionPtr>::iterator it = instructions.begin()
				; it != instructions.end()
				; ++it)
		cout << (++lineNum) << ": " << string(**it) << endl;
	cout << endl;

	CODE = instructions;
}

void queryForModerateFacts()
{
	using boost::make_shared;
	vector<ProducesInstructionPtr> nullVector;

	// ?- p(b, a).
	Clause queryClause( 
		Functor("p", 2),
		vector<ProducesInstructionPtr>({
			boost::make_shared<Clause>(Functor("b", 0), nullVector),
			boost::make_shared<Clause>(Functor("a", 0), nullVector)
		}));

	//problem: we have no fact to pump to getQueryInstruction
	//solution: we need to get rid of that dependancy. In order
	//to get rid of that dependancy, we'll need to put the
	//getNextAvailableRegister function somewhere else...
	//but keep in mind it deals with permenant variables...
	///hmmm we need to abstract it or something...
	//TERRIBLE setup... i dunno...
	VectorLock<InstructionPtr> vlip;
	RegisterTool rt__;
	vector<InstructionPtr> tempInstrs = queryClause.getQueryInstruction(rt__,vlip);

	cout << "Instructions for facts from pg 36 AND query p(b, a):" << endl;
	unsigned lineNum = 0;
	for(vector<InstructionPtr>::iterator it = CODE.begin()
				; it != CODE.end()
				; ++it)
		cout << (++lineNum) << ": " << string(**it) << endl;
	cout << endl;
}

void L0Test()
{
	X.resize(5);
	put_structure(boost::make_shared<Functor>("h",2), X[2]).trigger();	//?-	X3 = h
	set_variable(X[1]).trigger();							//				(Z,
	set_variable(X[4]).trigger();							//					W)
	put_structure(boost::make_shared<Functor>("f",1), X[3]).trigger();	//		X4 = f
	set_value(X[4]).trigger();								//				(W)
	put_structure(boost::make_shared<Functor>("p",3), X[0]).trigger();	//		X1 = p
	set_value(X[1]).trigger();								//				(Z,
	set_value(X[2]).trigger();								//					X3,
	set_value(X[3]).trigger();								//						X4).
	printHEAP();	
}

void L0CODETest()
{
	using boost::make_shared;
	CODE.push_back(make_shared<put_structure>(boost::make_shared<Functor>("h",2), X[2]));	//?-	X3 = h
	CODE.push_back(make_shared<set_variable>(X[1]));							//				(Z,
	CODE.push_back(make_shared<set_variable>(X[4]));							//					W)
	CODE.push_back(make_shared<put_structure>(boost::make_shared<Functor>("f",1), X[3]));	//		X4 = f
	CODE.push_back(make_shared<set_value>(X[4]));								//				(W)
	CODE.push_back(make_shared<put_structure>(boost::make_shared<Functor>("p",3), X[0]));	//		X1 = p
	CODE.push_back(make_shared<set_value>(X[1]));								//				(Z,
	CODE.push_back(make_shared<set_value>(X[2]));								//					X3,
	CODE.push_back(make_shared<set_value>(X[3]));								//						X4).

	for(vector<InstructionPtr>::iterator it = CODE.begin();
			it != CODE.end();
			++it)
		(*it)->trigger();
	
	printHEAP();	
}

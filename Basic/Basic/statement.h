/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface specified in Chapter 17, which is an
 * excellent model for the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include "evalstate.h"
#include "exp.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "parser.h"


/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */


class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

   Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

   virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

   virtual void execute(EvalState & state) = 0;

 /*
  * Method: getFlag
  * Usage: stmt->getFlag();
  * ----------------------------
  * This method returns "ended", so that we can check if the 
  * program has come to an end.
  */

   bool getFlag();

 /*
  * Method: nextLine
  * Usage: stmt->nextline();
  * ----------------------------
  * This method returns "next", so that we can check if the 
  * program needs to jump to the line in "IFStmt" or "GOTOStmt".
  */

   int nextline();

 /*
  * Method: reinit
  * Usage: stmt->reinit();
  * ----------------------------
  * This method reinitialize the Statement. If we run a program
  * that has "IFStmt" in it, maybe it will change the member "next"
  * in that statement, and when we run it again this may cause 
  * "IFStmt" to disfunction, so we need to reinitialize it.
  */   

   virtual void reinit();

protected:
	bool ended;
	int next;
};

class REMStmt :public Statement {
public:
	REMStmt();
	~REMStmt();
	void execute(EvalState &state);
};

class LETStmt :public Statement {
public:
	LETStmt(string &v, string &e);
	~LETStmt();
	void execute(EvalState &state);
private:
	string var;// use var to store the name of the variable, 
	string exp;// use exp to store the string of the expression.
};

class PRINTStmt :public Statement {
public:
	PRINTStmt(string &s);
	~PRINTStmt();
	void execute(EvalState &state);
private:
	string str;// use str to store the string of the expression.
};

class INPUTStmt :public Statement {
public:
	INPUTStmt(string v);
	~INPUTStmt();
	void execute(EvalState &state);
private:
	string var;// use var to store the name of the variable.
};

class ENDStmt :public Statement {
public:
	ENDStmt();
	~ENDStmt();
	void execute(EvalState &state);
};

class GOTOStmt :public Statement {
public:
	GOTOStmt(int &n);
	~GOTOStmt();
	void execute(EvalState &state);
};

/*
 * Class: IFStmt
 * ----------------
 * This class is used to represent an "IF" statement in a program.
 * We use *left and *right to store the two expressions, use op to
 * store the operation kind, and use ifnext to store the number 
 * after "THEN".
 */
class IFStmt :public Statement {
public:
	IFStmt(Expression *l, Expression *r, string o, int &n);
	~IFStmt();
	void execute(EvalState &state);
	void reinit();
private:
	Expression *left;
	Expression *right;
	string op;
	int ifnext;
};
#endif

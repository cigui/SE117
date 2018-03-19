/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/error.h"
using namespace std;

/* Implementation of the Statement class */

/* Well, in fact most of the definition here does the same
/* thing as those in "Basic.cpp", so I think you may check
/* the documentation in that file.
 */
Statement::Statement() {
	ended = false;
	next = -1;
}

Statement::~Statement() {
   /* Empty */
}

int Statement::nextline() {
	return next;
}

bool Statement::getFlag() {
	return ended;
}

void Statement::reinit(){}

REMStmt::REMStmt() {
	ended = false;
	next = -1;
}
REMStmt::~REMStmt(){}
void REMStmt::execute(EvalState &state) {}

LETStmt::LETStmt(string &v, string &e) {
	var = v;
	exp = e;
	ended = false;
	next = -1;
}
LETStmt::~LETStmt() {}
void LETStmt::execute(EvalState &state) {
	int val = 0;
	TokenScanner temp;
	temp.scanNumbers();
	temp.setInput(exp);
	Expression *e = parseExp(temp);
	val = e->eval(state);
	state.setValue(var, val);
}

PRINTStmt::PRINTStmt(string &s) {
	ended = false;
	next = -1;
	str = s;
}
PRINTStmt::~PRINTStmt() {}
void PRINTStmt::execute(EvalState &state) {
	TokenScanner temp;
	temp.scanNumbers();
	temp.setInput(str);
	Expression *exp = parseExp(temp);
	cout << exp->eval(state) << endl;
}

INPUTStmt::INPUTStmt(string v) {
	ended = false;
	next = -1;
	var = v;
}
INPUTStmt::~INPUTStmt(){}
void INPUTStmt::execute(EvalState &state) {
	while (true) {
		cout << " ? ";
		string str = getLine();
		unsigned int len = str.size();
		bool invalid_int = false;
		if (len = 1 && (str[0] == '-' || str[0] == '+')) {
			state.setValue(var, 0);
			break;
		}
		for (unsigned int i = 0; i < len; ++i) {
			if (str[i] - '0' < 0 || str[i] - '0' > 9) {
				invalid_int = true;
				break;
			}
		}
		if (!invalid_int) {
			try {
				int val = stringToInteger(str);
				state.setValue(var, val);
				break;
			}
			catch(ErrorException & ex) {}
		}
		cout << "INVALID NUMBER" << endl;
	}
}

ENDStmt::ENDStmt() {
	ended = true;
	next = -1;
}
ENDStmt::~ENDStmt(){}
void ENDStmt::execute(EvalState &state){}

GOTOStmt::GOTOStmt(int &n) {
	ended = false;
	next = n;
}
GOTOStmt::~GOTOStmt(){}
void GOTOStmt::execute(EvalState &state) {}

IFStmt::IFStmt(Expression *l, Expression *r, string o, int &n) {
	left = l;
	right = r;
	op = o;
	ifnext = n;
	ended = false;
	next = -1;
}
IFStmt::~IFStmt() {
	delete left;
	delete right;
}
void IFStmt::execute(EvalState &state) {
	int l = left->eval(state);
	int r = right->eval(state);
	switch (op[0]) {
	case'<':
		if (l < r) next = ifnext;
		break;
	case'=':
		if (l == r) next = ifnext;
		break;
	case'>':
		if (l > r) next = ifnext;
		break;
	default:
		error("SYNTAX ERROR");
	}
}
void IFStmt::reinit() {	next = -1;}// reinitialize this stmt

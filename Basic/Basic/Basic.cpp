/*
 * File: Basic.cpp
 * ---------------
 * Name: Zhou Xin
 * Student ID: 515030910073
 */

#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         cout << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
   vector<string> reservedW;//store  reserved words
   reservedW.push_back("REM");
   reservedW.push_back("LET");
   reservedW.push_back("PRINT");
   reservedW.push_back("INPUT");
   reservedW.push_back("END");
   reservedW.push_back("GOTO");
   reservedW.push_back("IF");
   reservedW.push_back("THEN");
   reservedW.push_back("LIST");
   reservedW.push_back("RUN");
   reservedW.push_back("CLEAR");
   reservedW.push_back("QUIT");
   reservedW.push_back("HELP");
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   string start = scanner.nextToken();
   if (isdigit(start[0])) {//  in this case, user tries inputing a program line
   	try {// use "try-catch" in case that it throws a wrong error like "Illegal ..."
			int lineNumber = stringToInteger(start);// get line number
			scanner.saveToken(start);// save token 
			program.addSourceLine(lineNumber, line);// so that we can get the whole line
			start = scanner.nextToken();// here take line number again
			Statement *stmt = NULL;
			string stype = scanner.nextToken();// "stype" stores  the type of the statement that user input.
			if (stype == "REM" || stype == "LET" || stype == "PRINT" || stype == "INPUT" || stype == "END") {
				switch (stype[0]) {// judge which type the statement is and then initialize it
				case 'R':// "REM"
					stmt = new REMStmt();
					break;
				case 'L': {// "LET"
					string name = scanner.nextToken();
					string op = scanner.nextToken();// "op" should be the operation "="
					if (op != "=") error("SYNTAX ERROR");
					string str = "";
					while (scanner.hasMoreTokens()) {
						str += scanner.nextToken();// get the expression as string
					}
					stmt = new LETStmt(name,str);
					break;
				}
				case 'P': {// "PRINT"
					string str = "";
					while (scanner.hasMoreTokens()) {
						str += scanner.nextToken();// get the expression as string
					}
					stmt = new PRINTStmt(str);
					break;
				}
				case 'I': {// "INPUT"
					string var = scanner.nextToken();// get the variable name
					if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
					stmt = new INPUTStmt(var);
					break;
				}
				case 'E':// "END"
					stmt = new ENDStmt();
					break;
				default:
					error("SYNTAX ERROR");
				}
				program.setParsedStatement(lineNumber, stmt);// set the statement into program
			}
			else if (stype == "GOTO" || stype == "IF") {
				switch (stype[0]) {// judge which type the statement is and then initialize it
				case'G': {// "GOTO n"
					string nextstr;
					nextstr = scanner.nextToken();// get the number of next line (n)
					if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
					int n = stringToInteger(nextstr);
					stmt = new GOTOStmt(n);
					break;
				}
				case'I': {// "IF exp op exp THEN n"
					TokenScanner temp;// use temp to get the exp before op
					string tempstr = scanner.nextToken();
					while (tempstr != "<" && tempstr != ">" && tempstr != "=") {
						temp.saveToken(tempstr);
						tempstr = scanner.nextToken();
					}
					string op = tempstr;
					TokenScanner reverse;// use reverse to reverse the string in temp
					while (temp.hasMoreTokens()) {
						tempstr = temp.nextToken();
						reverse.saveToken(tempstr);
					}
					string rightstr = "";
					tempstr = scanner.nextToken();
					while (tempstr != "THEN") {// get the exp after op
						rightstr += tempstr;
						tempstr = scanner.nextToken();
					}
					tempstr = scanner.nextToken();// get the number of next line (n)
					temp.setInput(rightstr);
					int n = stringToInteger(tempstr);
					Expression *left = parseExp(reverse);
					Expression *right = parseExp(temp);
					stmt = new IFStmt(left, right, op, n);
					break;
				}
				default:
					error("SYNTAX ERROR");
				}
				program.setParsedStatement(lineNumber, stmt);// set the statement into program
			}
			else if (stype == "") {
				program.removeSourceLine(lineNumber);// delete line
			}
			else {
				error("SYNTAX ERROR");
			}
		}
		catch(ErrorException & ex) {
			error("SYNTAX ERROR");
		}
	}
	else {// in this case, we don't need to put lines into program
		if (start == "RUN") {// run the program
			if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
			int lineNum = program.getFirstLineNumber();// get first line number
			Statement *stmt = NULL;
			while (true) {
				stmt = program.getParsedStatement(lineNum);// get stmt
				if (stmt == NULL) error("LINE NUMBER ERROR");// if stmt = NULL, then lineNum must be not in the map in "program"
				stmt->execute(state);
				if (stmt->nextline() != -1) {// in this case, stmt = IFStmt or GOTOStmt
					lineNum = stmt->nextline();
					stmt->reinit();// reinitialize IFStmt (make sure next = -1 so that it won't bother when this stmt comes again)
					continue;
				}
				if (stmt->getFlag()) break;// if getFlag() == true, then the program runs to an end.
				lineNum = program.getNextLineNumber(lineNum);
				if (lineNum == -1) break;// if lineNum == -1, then we've come to the final line of the program
			}
		}
		else if (start == "LIST") {// print all the lines that have been input
			if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
			int lineNum = program.getFirstLineNumber();
			while (lineNum != -1) {
				string line = program.getSourceLine(lineNum);
				cout << line << endl;
				lineNum = program.getNextLineNumber(lineNum);
			}
		}
		else if (start == "QUIT") {
			if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
			exit(0);
		}
		else if (start == "INPUT") {
			if (!scanner.hasMoreTokens()) error("SYNTAX ERROR");
			string name = scanner.nextToken();
			if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
			while (true) {
				cout << " ? ";
				string temp = getLine();
				unsigned int len = temp.size();
				bool invalid_n = false;
				if (len == 1 && (temp[0] == '-' || temp[0] == '+')) {// if the user only type "+" or "-", set it as 0
					state.setValue(name, 0);
					break;
				}
				for (int i = 0; i < len; ++i) {
					if (temp[i] - '0' < 0 || temp[i] - '0' > 9) {// this is an invalid number
						if (i == 0 && (temp[0] == '-' || temp[0] == '+')) continue;
						invalid_n = true;
						break;
					}
				}
				if (!invalid_n) {// if this is a valid number
					try {// use "try-catch" to ensure that we continue to prompt the user to input
						int val = stringToInteger(temp);
						state.setValue(name, val);
						break;
					}
					catch (ErrorException & ex) {}
				}
				cout << "INVALID NUMBER" << endl;
			}
		}
		else if (start == "LET") {
			string name = scanner.nextToken();
			bool reserved = false;
			for (unsigned int i = 0; i < reservedW.size(); ++i){// check if it is a reserved word
				if (reservedW[i] == name){
					reserved = true;
					break;
				}
			}
			if (reserved) error("SYNTAX ERROR");
			string op = scanner.nextToken();
			if (op != "=") error("SYNTAX ERROR");
			Expression *exp = parseExp(scanner);
			int val = exp->eval(state);
			state.setValue(name, val);// set value
		}
		else if (start == "PRINT") {
			Expression *exp = parseExp(scanner);
			int val = exp->eval(state);// get value
			cout << val << endl;// print value
		}
		else if (start == "") {}
		else if (start == "CLEAR") {
			program.clear();
			EvalState newstate;
			state = newstate;// clear the variables
		}
		else if (start == "HELP") {// HELP
			cout << "syntax error...nope! This is a BASIC interpreter :)" << endl;
		}
		else error("SYNTAX ERROR");
	
   }
}

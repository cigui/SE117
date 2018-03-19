/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
using namespace std;

Program::Program() {
   // Empty
}

Program::~Program() {
   // Empty
}

void Program::clear() {
   lines.clear();
   statements.clear();
}

void Program::addSourceLine(int lineNumber, string line) {
   if (lines.count(lineNumber) == 1) {
      lines[lineNumber] = line;
      statements.erase(lineNumber);// delete the original stmt of this number
   }
   else {
      lines[lineNumber] = line;
   }
}

void Program::removeSourceLine(int lineNumber) {
   lines.erase(lineNumber);
   statements.erase(lineNumber);
}

string Program::getSourceLine(int lineNumber) {
   return lines[lineNumber];
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
   if (lines.count(lineNumber) != 1) error("LINE NUMBER ERROR");
   if (statements.count(lineNumber) == 1) statements.erase(lineNumber);
   statements[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
   if (statements.count(lineNumber) != 1)  return NULL;
   Statement *stmt;
   stmt = statements[lineNumber];
   return stmt;
}

int Program::getFirstLineNumber() {
   if (lines.empty()) return -1;
   map<int, string>::iterator it = lines.begin();
   return it->first;
}

int Program::getNextLineNumber(int lineNumber) {
   map<int, string>::iterator it = lines.find(lineNumber);
   ++it;
   if (it == lines.end()) return -1;
   return it->first;
}
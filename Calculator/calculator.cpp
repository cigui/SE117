/***************************************************
Homework 3: Calculor
Author:Zhou Xin
Student ID:515030910073
Date:2016.3.26
***************************************************/
#include <iostream>
#include<string>
#include<vector>
#include<cmath>
using namespace std;

const char number = '8';//t.kind == number means that t is a number Token
const char quit = 'q';//t.kind == quit means that t is a quit Token
const char print = ';';//t.kind == print means that t is a print Token
const string prompt = "> ";//used to prompt user to input
const string result = "= ";//used to indicate that what follows is a result

class err {};
class Token {
public:
	char kind;
	double value;
	Token(char ch)
		:kind(ch), value(0) { }
	Token(char ch, double val)
		:kind(ch), value(val) { }
};
class Token_stream {
public:
	Token_stream();
	Token get();//get token from stream
	void putback(Token t);//put a Token back into the stream
	void ignore(char c);//discard characters up to and including a c
private:
	bool full;//is there a Token in the buffer?
	Token buffer;//here is where we keep a Token put back using putback()
};

Token_stream::Token_stream()
	:full(false), buffer(0)
{
}

void Token_stream::putback(Token t)
{
	if (full) throw err();
	buffer = t;
	full = true;
}
Token Token_stream::get()
{
	if (full)
	{
		full = false;
		return buffer;
	}

	char ch;
	cin >> ch;

	switch (ch)
	{
	case quit:
	case print:
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '!':
		return Token(ch);
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	{
		cin.putback(ch);//put the number back so we can read it again as double
		double val;
		cin >> val;
		return Token(number, val);
	}
	default: throw err();
	}
}


Token_stream ts;
double expression();//deal with '+' and '-'
double second();//deal with '!'(factorial)
double primary()// primary is a number or '('+expression+')'
{
	Token t = ts.get();
	switch (t.kind) {
	case '(': {
		double d = expression();
		t = ts.get();
		if (t.kind != ')') {
			ts.putback(t);//enable the calculator to continue to output prompt after throwing err()
			throw err();
		}
		return d;
	}
	case '+': return primary();
	case '-': return -second();//in case that we have a second next.
	case number: return t.value;
	default: throw err();
	}
}

double second() {//deal with '!'(factorial)
	double left = primary();
	Token t = ts.get();
	switch (t.kind) {
	case '!': {
		int i = int(left);
		if (i != left) throw err();
		if (i == 0) return 1;
		if (i < 0) throw err();
		for (int k = 1; k < i; ++k) left *= k;
		return left;
	}
	default:
		ts.putback(t);
		return left;
	}
}

double term()//deal with '*' , '/'and '%'
{
	double left = second();
	Token t = ts.get();
	while (true)
	{
		switch (t.kind)
		{
		case '*':
			left *= second();
			t = ts.get();
			break;
		case '/': {
			double d = second();
			if (d == 0) throw err();
			left /= d;
			t = ts.get();
			break;
		}
		case '%': {
			double d = second();
			int i1 = int(left);
			if (i1 != left) throw err();
			int i2 = int(d);
			if (i2 != d || i2 == 0) throw err();
				left = i1 % i2;
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression()//deal with '+' and '-'
{
	double left = term();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '+':
			left += term();
			t = ts.get();
			break;
		case '-':
			left -= term();
			t = ts.get();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

void Token_stream::ignore(char c) 
    // c reresents the kind of Token
{
	//first look in buffer:
	if (full&&c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	//now search input:
	char ch = 0;
	while (cin >> ch)
		if (ch == c) return;
}

void calculate()
{
	double val = 0;
	while (cin)
	{
		try {
			cout << prompt;
			Token t = ts.get();
			while (t.kind == print) t = ts.get();
			if (t.kind == quit) break;
			ts.putback(t);
			val = expression();
			cout << result << val << endl;
		}
		catch (...) {
			cerr << "error" << endl;
			ts.ignore(print);//discard characters up to and including a c
		}
	}
}

int main()
{
	try {
		calculate();
		return 0;
	}
	catch (...) {
		cerr << "error" << endl;
		return 1;
	}
}


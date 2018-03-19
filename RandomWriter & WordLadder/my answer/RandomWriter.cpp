/*
* File: RandomWriter.cpp
* ----------------------
* Name:Zhou Xin
* Student ID:515030910073
* This file is the starter project for the random writer problem
* on Assignment #2.
*/

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
using namespace std;
#define MAX_CHAR_NUM 2000

//get filename
void read_file(ifstream& file_input);

//get order number
int read_order();

//build model
void model_read(ifstream& file_input, const int& order, map<string, vector<char> >& model);

//write
void random_write(const int& order, map<string, vector<char> >& model);

void read_file(ifstream& file_input) {
	while (true) {
		cout << "Please enter filename containing source text:(enter ':q' to quit)";//prompt
		string file_name = "";
		while (file_name == "") {//this avoids repeating printing prompt 
			getline(cin, file_name);
		}
		if (file_name == ":q") exit(0);
		file_input.open(file_name.c_str());
		if (file_input.is_open()) break;//if we can't open the file, go to next loop
	}
};

int read_order() {
	int order = 1;
	cout << "What order to analysis ? (a number from 1 to 10)";
	cin >> order;
	return order;
}

void model_read(ifstream& file_input, const int& order, map<string, vector<char> >& model) {
	cout << "Analyzing ... please wait.\n";
	string result = "";
	string temp;// string function "append" doesn't allow the argument to be a char, so I use temp.
	char ch;
	while (file_input.get(ch)) {// get every char from the file and put it into "result".
		temp = ch;
		result.append(temp);
	}
	string seed(order, 'a');// seed is a string we get from "result", whose length equals to "order".
	int len = result.size();
	for (int i = 0; i < len - order; ++i) {// get every seed from "result"
		for (int j = 0; j < order; ++j) seed[j] = result[i + j];
		if (model.find(seed) == model.end()) model[seed] = { result[i + order] };// if there isn't a 
		// seed that is the same with this one, we put it into the map and set the value for it.
		else model[seed].push_back(result[i + order]);// if there is, simply add the new char into
		// this seed's vector.
	}
}

void random_write(const int& order, map<string, vector<char> >& model) {
	vector<string> first_seeds;// this stores the first seed to put into result
	unsigned int count = 0;// "count" is a record of the seeds' frequency
	map<string, vector<char>>::iterator it;
	for (it = model.begin(); it != model.end(); ++it) {// determine which seed(s) 
		// occurs most frequently in the source
		if (it->second.size() > count) {// find a seed that occurs more frequently than 
			// those in "first_seeds"
			first_seeds.clear();// clear the vector
			first_seeds.push_back(it->first);// put the new seed into vector
			count = it->second.size();// update "count"
		}
		else if (it->second.size() == count) first_seeds.push_back(it->first);// add new seed
	}
	srand(time(0));
	string result = (first_seeds[rand() % first_seeds.size()]);// get a seed from the vector randomly
	int len = result.size();
	string temp;//use the same strategy as that in "model_read" to use function "append"
	char ch;
	while (len < MAX_CHAR_NUM) {
		if (model.find(result.substr(len - order, order)) != model.end()) {// current seed is found 
			// in the model
			vector<char> chars = model.find(result.substr(len - order, order))->second;//get the 
			// possible characters after this seed
			ch = chars[rand() % chars.size()];//get a character from "chars" randomly
			temp = ch;
			result.append(temp);
			++len;//update the length of "result"
		}
		else break;
	}
	cout << result << endl;
}

int main() {
	while (true) {
		ifstream file_input;
		read_file(file_input);
		const int order = read_order();
		map<string, vector<char>> model;
		model_read(file_input, order, model);
		random_write(order, model);
	}
	return 0;
}

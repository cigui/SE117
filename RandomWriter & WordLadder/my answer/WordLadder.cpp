/*
* File: WordLadder.cpp
* --------------------
* Name:Zhou Xin
* Student ID:515030910073
* This file is the starter project for the word ladder problem.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <queue>
#include <vector>
using namespace std;

//find the dest and init
void show_ans(vector<string>& ans);

//input and start function
void input_words(string& start, string& dest);

// read lexicon from file
void read_english(set<string>& english);

void show_ans(vector<string>& ans) {
	cout << "Found ladder:";
	cout << ans[0];
	for (unsigned int i = 1; i < ans.size(); i++) {
		cout << "->" << ans[i];
	}
	cout << endl << endl;
}

void input_words(string& start, string& dest) {
	cout << "Enter start word (enter ':q' to quit):";
	cin >> start;
	if (start == ":q") {
		exit(0);
	}
	cout << "Enter destination word:";
	cin >> dest;
}

void read_english(set<string>& english) {
	while (true) {
		cout << "Please enter filename containing source text:";
		string file_name;
		getline(cin, file_name);
		ifstream file_input(file_name.c_str());
		if (file_input.is_open()) {
			string temp_word;
			while (getline(file_input, temp_word)) {
				english.insert(temp_word);
			}
			break;
		}
	}
}

int main() {
	char letters[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
	set<string> english;
	read_english(english);
	while (true) {
		string start, dest;
		input_words(start, dest);
		int len = start.size();
		if (len != dest.size()) {//if the length of "start" is different from that of "dest",
			cout << "No Ladder found." << endl;//then there is no ladder.
			continue;
		}
		queue<vector<string>> ladders;//Create an empty queue
		vector<string> lad;
		lad.push_back(start);//Add the start word to a new ladder
		ladders.push(lad);//Add the ladder to the queue
		set<string> used_words;
		used_words.insert(start);
		set<string>::iterator it;
		set<string>::iterator uit;
		bool showed = false;//use "showed" to avoid duplicate output
		set<string> new_english;//use "new_english" as lexicon which stores words only with 
								//the same length as "start" and "dest"
		for (it = english.begin(); it != english.end(); ++it) {
			string comp = *it;
			if (comp.size() != len) continue;
			new_english.insert(comp);
		}
		while (!ladders.empty()) {//the queue is not empty
			lad = ladders.front();
			ladders.pop();//Dequeue the first ladder from the queue
			string now = lad.back();//get the final word in this ladder
			if (now == dest) {//the final word in this ladder is the destination word
				show_ans(lad);//Return this ladder as the solution
				showed = true;
				break;
			}
			for (int i = 0; i < len; ++i) {
				string copy = now;//copy the final word, in order not to change it
				for (int j = 0; j < 26; ++j) {
					copy[i] = letters[j];//change one letter, and then check if the new word has been used or isn't in lexicon
					if (used_words.find(copy) == used_words.end() && new_english.find(copy) != new_english.end()) {
						vector<string> newl = lad;//Create a copy of the current ladder
						newl.push_back(copy);//Add the new word to the end of the copy
						used_words.insert(copy);//Add the new word to the end of "used_words"
						ladders.push(newl);// Add the new ladder to the end of the queue
					}
				}
			}
		}
		if (!showed) cout << "No Ladder found." << endl;
	}
	return 0;
}

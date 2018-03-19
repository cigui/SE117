/*
* File: WordLadder.cpp
* --------------------
* [TODO: fill in your name and student ID]
* Name:
* Student ID:
* This file is the starter project for the word ladder problem.
* [TODO: extend the documentation]
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
    for (int i = 1; i < ans.size(); i++) {
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
  while(true) {
    cout << "Please enter filename containing source text:";
    string file_name;
    cin >> file_name;
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
  // TODO your code
  return 0;
}

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <stack>
#include "stdlib.h"
#include <algorithm>
using namespace std;

void naiveCopy(string inputFilename, string outputFilename) {
  ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  char c;
  while (ifs.get(c)) ofs.put(c);
  ofs.close();
  ifs.close();
}

struct Node {
  Node* LC;
  Node* RC;
  Node* parent;
  int weight;
  unsigned char ch;
  Node(Node* L = NULL, Node* R = NULL, Node* p = NULL,
    unsigned char c = '0', int w = 0) :
    LC(L), RC(R), parent(p), ch(c), weight(w) { };
  bool operator< (Node const& n)const { return weight < n.weight; }
  ~Node() { delete LC, RC, parent; }
};

bool check(string inputFilename){
  ifstream check(inputFilename.c_str(), ios::in | ios::binary);
  char checkc;
  check.read((char *)&checkc, sizeof(char));
  if (check.eof()){
    check.close();
    return true;
  }
  check.close();
  return false;
}

multimap<int, Node*> getForest(map<unsigned char, int> &nodes) {
  // This function creates Node with the unsigned char and int
  // in the vector.
  multimap<int, Node*> forest;
  unsigned char c = '0';
  int w = 0;
  map<unsigned char, int>::iterator it = nodes.begin();
  while (it != nodes.end()) {
    c = it->first;
    w = it->second;
    Node* n = new Node(NULL, NULL, NULL, c, w);
    forest.insert(pair<int, Node*>(w, n));
    ++it;
  }
  return forest;
};

void getRoot(multimap<int, Node*> &forest) {
  // This function creates a Huffman Tree with the Nodes
  // in the map, and make the root of the tree the only
  // element of the map.
  multimap<int, Node*>::iterator it = forest.begin();
  while (forest.size() != 1) {
    it = forest.begin();
    Node* n1 = NULL;
    Node* n2 = NULL;
    n1 = it->second;
    ++it;
    n2 = it->second;
    int pweight = n1->weight + n2->weight;
    Node* np = new Node(n1, n2, NULL, '^', pweight);
    n1->parent = np;
    n2->parent = np;
    forest.erase(forest.begin());
    forest.erase(forest.begin());
    forest.insert(pair<int, Node*>(pweight, np));
  }
}

vector<Node*> getTravPreVec(multimap<int, Node*> &forest) {
  // This function gets the Preorder Traversal of the tree.
  vector<Node*> nodesVec;
  Node* root = NULL;
  root = forest.begin()->second;
  stack<Node*> s;
  while (true) {
    while (root->LC != NULL) {
      s.push(root->RC);
      nodesVec.push_back(root);
      root = root->LC;
    }
    nodesVec.push_back(root);
    if (s.empty()) break;
    root = s.top();
    s.pop();
  }
  return nodesVec;
}

string getTPCode(vector<Node*> &nodesVec) {
  // This function gets the string of the Preorder Traversal
  // of the tree, using '1' to represent leaf nodes.
  string result = "";
  int size = nodesVec.size();
  for (int i = 0; i < size; ++i) {
    if (nodesVec[i]->LC != NULL) result.append("0");
    else result.append("1");
  }
  return result;
}

vector<unsigned char> getTPChar(vector<Node*> &nodesVec) {
  // This function gets the unsigned chars of the leaf nodes
  // in Preorder Traversal.
  vector<unsigned char> result;
  int size = nodesVec.size();
  for (int i = 0; i < size; ++i) {
    if (nodesVec[i]->LC == NULL) {
      result.push_back(nodesVec[i]->ch);
    }
  }
  return result;
}

map<unsigned char, string> getCodeMap(vector<Node*> &nodesVec) {
  // This function gets a map, the keys are the unsigned char of the nodes
  // and the values are the "01" string that represent their Huffman code.
  map<unsigned char, string> codeMap;
  int size = nodesVec.size();
  for (int i = 0; i < size; ++i) {
    if (nodesVec[i]->LC != NULL) continue;
    else {
      Node* cur = nodesVec[i];
      unsigned char c = cur->ch;
      string curCode = "";
      while (cur->parent != NULL) {
        if (cur->parent->LC == cur) curCode.append("0");
        else curCode.append("1");
        cur = cur->parent;
      }
      reverse(curCode.begin(), curCode.end());
      codeMap[c] = curCode;
    }
  }
  return codeMap;
}

map<unsigned char, int> getNodes(string inputFilename) {
  // This function gets the unsigned char in the file and record
  // their frequency.
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  char c;
  map<unsigned char, int> nodes;
  while (ifs.get(c)) {
    if (nodes.count((unsigned char)c) == 1) {
      nodes[(unsigned char)c] += 1;
    }
    else {
      nodes[(unsigned char)c] = 1;
    }
  }
  ifs.close();
  return nodes;
}

void compress(string inputFilename, string outputFilename) {
  // Firstly, we use "check" to check if this is an empty file.
  if (check(inputFilename)){// If it is, just copy it and return.
    naiveCopy(inputFilename, outputFilename);
    return;
  }
  map<unsigned char, int> nodes = getNodes(inputFilename);
  multimap<int, Node*> forest = getForest(nodes);
  getRoot(forest);
  vector<Node*> nodesVec = getTravPreVec(forest);
  map<unsigned char, string> codeMap = getCodeMap(nodesVec);
  string travPreCode = getTPCode(nodesVec);
  vector<unsigned char> travPreChar = getTPChar(nodesVec);
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
  // In order to put it into the file, we should make sure the 
  // length of the "01" string can be devided by 8 with no
  // remainder, so we can change the string into unsigned char.
  int r1 = travPreCode.size() % 8;
  if (r1 != 0) {
    r1 = 8 - r1;
    travPreCode.append(r1, '0');
  }
  char c;
  string fileCode = "";
  ifs.seekg(0);
  while (ifs.get(c)) fileCode.append(codeMap[(unsigned char)c]);
  int r2 = fileCode.size() % 8;
  if (r2 != 0) {
    r2 = 8 - r2;
    fileCode.append(r2, '0');
  }
  int nodeNum = (int) nodes.size();
  // Then we write the number of the node, the remainder of
  // travPreCode (devided by 8), and the remainder of fileCode
  // (devided by 8).
  // In case that the number of the node grows to 256(the 
  // biggest number of the unsigned chars) and cause trouble
  // later when we read it again, we need to write it as "1" and 
  // "255".(In fact, this does trouble me in "trace07.bin")
  if (nodeNum == 256) {
    int i = 1;
    ofs.write((char *)& i, sizeof(char));
    nodeNum--;
  }
  ofs.write((char *)&nodeNum, sizeof(char));
  ofs.write((char *)&r1, sizeof(char));
  ofs.write((char *)&r2, sizeof(char));
  // Use bitwise operation to change the "01" string into 
  // unsigned char and put it into the file.
  unsigned char ch = 0;
  int b = 0;
  for (unsigned int i = 0; i < travPreCode.size(); ++i) {
    ch <<= 1;
    b++;
    if (travPreCode[i] == '1')
      ch = ch | 1;
    if (b == 8) {
      ofs.write((char*)&ch, sizeof(char));
      b = 0;
      ch = 0;
    }
  }
  for (unsigned int i = 0; i < travPreChar.size(); ++i) {
    ofs.write((char *)& travPreChar[i], sizeof(char));
  }
  b = 0;
  ch = 0;
  for (unsigned int i = 0; i < fileCode.size(); i++) {
    ch <<= 1;
    b++;
    if (fileCode[i] == '1')
      ch = ch | 1;
    if (b == 8) {
      ofs.write((char*)&ch, sizeof(char));
      b = 0;
      ch = 0;
    }
  }
  ofs.close();
  ifs.close();
};

string get01str(vector<unsigned char> &v) {
  // This function gets the "01" string of the unsigned
  // char in the vector.
  string str = "";
  unsigned char ch = 0;
  for (unsigned int i = 0; i < v.size(); ++i) {
    for (int j = 7; j >= 0; j--) {
      ch = v[i];
      str.append(1, char(((ch >> j) & 1) + '0'));
    }
  }
  return str;
}

vector<Node*> getTPNodes(vector<Node*> &node) {
  // This function gets the nodes of the Huffman tree
  // in Preorder Traversal.
  vector<Node*> nodesVec;
  Node* root = NULL;
  root = node[0];
  stack<Node*> s;
  while (true) {
    while (root->LC != NULL) {
      s.push(root->RC);
      nodesVec.push_back(root);
      root = root->LC;
    }
    nodesVec.push_back(root);
    if (s.empty()) break;
    root = s.top();
    s.pop();
  }
  return nodesVec;
}

map<string, unsigned char> getCodeMap_d(vector<Node*> &nodesVec) {
  // This function gets a map, the keys are the "01" string of the unsigned
  // chars, and the values are the unsigned chars.
  map<string, unsigned char> codeMap;
  int size = nodesVec.size();
  for (int i = 0; i < size; ++i) {
    if (nodesVec[i]->LC != NULL) continue;
    else {
      Node* cur = nodesVec[i];
      unsigned char c = cur->ch;
      string curCode = "";
      while (cur->parent != NULL) {
        if (cur->parent->LC == cur) curCode.append("0");
        else curCode.append("1");
        cur = cur->parent;
      }
      reverse(curCode.begin(), curCode.end());
      codeMap[curCode] = c;
    }
  }
  return codeMap;
}

bool subTree(vector<vector<Node*> > &vec, int i){
  // This function judges if we can make a sub tree in position i.
  return (!vec[i].empty() && !vec[i - 1].empty() && vec[i - 2].empty());
}

void decompress(string inputFilename, string outputFilename) {
  // Similar with the beginning of "compress"
  if (check(inputFilename)){
    naiveCopy(inputFilename, outputFilename);
    return;
  }
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
  int nodeNum, r1, r2;
  // nodeNum, r1 and r2 are exactly the same as those in "compress"
  unsigned char ch;
  ifs.read((char *)&ch, sizeof(char));
  if (ifs.eof()) {
    ifs.close();
    ofs.close();
    return;
  }
  nodeNum = int(ch);
  ifs.read((char *)&ch, sizeof(char));
  r1 = int(ch);
  if (nodeNum == 1 && r1 == 255){
  // Reflect the "1" and "255" we wrote in "compress".
    nodeNum = 256;
    ifs.read((char *)&ch, sizeof(char));
    r1 = int(ch);
  }
  ifs.read((char *)&ch, sizeof(char));
  r2 = int(ch);
  int TPCodelength = (nodeNum * 2 - 1 + r1) / 8;
  vector<unsigned char> TPCode;
  int TPCharlength = nodeNum;
  vector<unsigned char> TPChar;
  vector<unsigned char> fileCode;
  // Get the preoder travelsal code, the chars and the
  // code of the file.
  for (int i = 0; i < TPCodelength; ++i) {
    ifs.read((char *)&ch, sizeof(char));
    TPCode.push_back(ch);
  }
  for (int i = 0; i < TPCharlength; ++i) {
    ifs.read((char *)&ch, sizeof(char));
    TPChar.push_back(ch);
  }
  ifs.read((char *)&ch, sizeof(char));
  while (!ifs.eof()) {
    fileCode.push_back(ch);
    ifs.read((char *)&ch, sizeof(char));
  }
  // Get their "01" string
  string TPCodeStr = get01str(TPCode);
  string TPCharStr = get01str(TPChar);
  string fileCodeStr = get01str(fileCode);
  // Delete the remainder of the string.
  if (r1 != 0) {
    for (int i = 0; i < r1; ++i) TPCodeStr.erase(TPCodeStr.end() - 1);
  }
  if (r2 != 0) {
    for (int i = 0; i < r2; ++i) fileCodeStr.erase(fileCodeStr.end() - 1);
  }
  vector<unsigned char> chars;
  vector<Node*> nodesVec;
  int b = 0;
  // Get the chars.
  for (unsigned int i = 0; i < TPCharStr.size(); ++i) {
    ch <<= 1;
    b++;
    if (TPCharStr[i] == '1')
      ch = ch | 1;
    if (b == 8) {
      chars.push_back(ch);
      b = 0;
      ch = 0;
    }
  }
  vector<vector<Node *> > nodes;
  vector<Node *> temp;
  int count = 0;
  // Create the Nodes.
  for (unsigned int i = 0; i < TPCodeStr.size(); ++i) {
    if (TPCodeStr[i] != '0') {
      Node *n = new Node(NULL, NULL, NULL, chars[count], 1);
      count++;
      temp.push_back(n);
      nodes.push_back(temp);
      temp.pop_back();
    }
    else nodes.push_back(temp);
  }
  // Reproduce the Huffman tree.
  while (nodes.size() != 1) {
    int curP = nodes.size() - 1;
    while (!subTree(nodes, curP)) curP--;
    Node *p = new Node(nodes[curP - 1][0], nodes[curP][0], NULL, '^', 1);
    nodes[curP - 1][0]->parent = p;
    nodes[curP][0]->parent = p;
    temp.push_back(p);
    nodes.erase(nodes.begin() + curP - 2, nodes.begin() + curP + 1);
    nodes.insert(nodes.begin() + (curP - 2), temp);
    temp.pop_back();
  }
  vector<Node *> TPNodes = getTPNodes(nodes[0]);
  map<string, unsigned char> codeMap = getCodeMap_d(TPNodes);
  string tempStr = "";
  // Use tempStr and codeMap to get all the unsigned chars in order
  // and put them into the file.
  while (fileCodeStr.size() != 0) {
    tempStr.append(1, fileCodeStr[0]);
    fileCodeStr.erase(fileCodeStr.begin());
    if (codeMap.find(tempStr) != codeMap.end()) {
      unsigned char ch = codeMap[tempStr];
      ofs.write((char *)&ch, sizeof(char));
      tempStr = "";
    }
  }
  ifs.close();
  ofs.close();
}

void useage(string prog) {
  cerr << "Useage: " << endl
    << "    " << prog << "[-d] input_file output_file" << endl;
  exit(2);
}

int main(int argc, char* argv[]) {
  int i;
  string inputFilename, outputFilename;
  bool isDecompress = false;
  for (i = 1; i < argc; i++) {
    if (argv[i] == string("-d")) isDecompress = true;
    else if (inputFilename == "") inputFilename = argv[i];
    else if (outputFilename == "") outputFilename = argv[i];
    else useage(argv[0]);
  }
  if (outputFilename == "") useage(argv[0]);
  if (isDecompress) decompress(inputFilename, outputFilename);
  else compress(inputFilename, outputFilename);
  return 0;
}

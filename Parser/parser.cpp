#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <memory>
#include <array>

class Node {
public:
  int type;
  std::string value;
  std::vector<Node*> children;
};

class StackEntry {
public:
  Node *node;
  int state;

};

class Parser {
public:

};

int main() {
  return 0;
}


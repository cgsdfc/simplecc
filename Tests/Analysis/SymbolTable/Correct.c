const int ConstantInt = 1;
const char ConstantChar = 'a';

int IntArray[2], IntVar;
char CharArray[2], CharVar;

void Test1 {
  const int ConstantInt = 2;
  const char ConstantChar = 'b';
}

void Test2 {
  int IntVar;
  char CharArray[3];
}

void Test3(int Arg) {
  Arg = 1;
}

void main() {
  IntVar = 1;
  Printf(IntVar);
  CharArray[0] = 'a';
  Test1;
  Test2;
}

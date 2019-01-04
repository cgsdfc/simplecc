void Func(int i) {}
char CharFunc {}
int IntFunc {}

void AssignToConst(int a) {
  const int IntVal = 1;
  const char CharVal = 'a';
  IntVal = 2;
  CharVal = 'b';
}

void main() {
  const int IntVal = 1;
  const char CharVal = 'a';
  int IntVar;
  char CharVar;
  int IntArray[10];
  char CharArray[10];

  IntVal = 1;
  IntArray = 1;
  Func = 1;

  IntVar = IntArray;
  CharVar = IntArray;

  IntVar = CharVal;
  CharVar = IntVal;
  IntArray[0] = CharVal;
  CharArray[0] = IntVal;
  CharVar = IntFunc;
  IntVar = CharFunc;

  IntVar = 1;
  IntVar = IntFunc;
  IntVar = IntArray[0];
  IntArray[0] = 1;
  IntArray[0] = IntVar;
  IntArray[0] = IntFunc;

  CharVar = 'a';
  CharVar = CharFunc;
  CharVar = CharArray[0];
  CharArray[0] = 'a';
  CharArray[0] = CharVar;
  CharArray[0] = CharFunc;
}
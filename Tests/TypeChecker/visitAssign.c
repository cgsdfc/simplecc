void Func(int i) {}
char CharFunc {}
int IntFunc {}

void main() {
  const int IntConst = 1;
  const char CharConst = 'a';
  int IntVar;
  char CharVar;
  int IntArray[10];
  char CharArray[10];

  IntConst = 1;
  IntArray = 1;
  Func = 1;

  IntVar = IntArray;
  CharVar = IntArray;

  IntVar = CharConst;
  CharVar = IntConst;
  IntArray[0] = CharConst;
  CharArray[0] = IntConst;
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

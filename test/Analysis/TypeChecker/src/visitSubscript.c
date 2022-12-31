void VoidFunc {}
int IntFunc {}
char CharFunc { return('c'); }

void main() {
  const int IntConst = 1;
  const char CharConst = 'a';
  int IntVar;
  char CharVar;
  int IntArray[2];
  char CharArray[2];

  IntConst[0] = 1;
  IntVar = IntConst[0];
  VoidFunc[0] = 1;
  IntVar = VoidFunc[0];

  IntVar = IntArray[CharConst];
  IntVar = IntArray[CharFunc];
  IntVar = IntArray[VoidFunc];
  IntVar = IntArray[IntArray];
  IntVar = IntArray[CharVar];
  IntVar = IntArray[CharArray[0]];

  IntVar = IntArray[0];
  IntVar = IntArray[IntConst];
  IntVar = IntArray[('a')];
  IntVar = IntArray[IntFunc];
  IntVar = IntArray[IntArray[0]];

}

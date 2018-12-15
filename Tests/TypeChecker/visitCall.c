void VoidFuncII(int a1, int a2) {

}

void VoidFunc {

}

void VoidFunc_IC(int a1, char a2) {

}

void main() {
  int IntVar;
  char CharVar;
  char CharArray[2];

  IntVar;
  IntVar(1);
  CharArray;
  CharArray(1);

  VoidFuncII;
  VoidFuncII(1);
  VoidFuncII(1, 2, 3);
  VoidFunc(1);

  VoidFuncII(CharVar, IntVar);
  VoidFunc_IC(VoidFunc, CharVar);
  VoidFuncII(VoidFunc, VoidFunc);

  VoidFunc;
  VoidFuncII(1, 1);
  VoidFunc_IC(1, CharVar);
  VoidFuncII((CharVar), (CharVar));
  VoidFunc_IC((CharVar), CharVar);
}

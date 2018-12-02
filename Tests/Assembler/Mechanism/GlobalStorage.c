int IntVar_1;
char CharArray[2];

void main() {
  IntVar_1 = 1;
  CharArray[0] = 'a';
  CharArray[1] = 'b';

  Printf("Dump Global Variable:");
  Printf("Expect 1: ", IntVar_1);

  Printf("Dump Global Array:");
  Printf("Expect a: ", CharArray[0]);
  Printf("Expect b: ", CharArray[1]);
}

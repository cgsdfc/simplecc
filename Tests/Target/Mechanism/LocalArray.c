void TestLocalArray {
  char CharArray[3];

  CharArray[0] = 'a';
  CharArray[1] = 'b';
  CharArray[2] = 'c';

  Printf("Dump Local Array:");
  Printf("Expect a: ", CharArray[0]);
  Printf("Expect b: ", CharArray[1]);
  Printf("Expect c: ", CharArray[2]);
}

void main() {
  TestLocalArray;
}
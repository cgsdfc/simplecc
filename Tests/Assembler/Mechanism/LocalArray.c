void TestCharArray {
  char CharArray[3];

  CharArray[0] = 'a';
  CharArray[1] = 'b';
  CharArray[2] = 'c';

  Printf("at 0: ", CharArray[0]);
  Printf("at 1: ", CharArray[1]);
  Printf("at 2: ", CharArray[2]);
}

void main() {
  Printf("TestCharArray:");
  TestCharArray;
}

char CharArray[3];

void main() {
  CharArray[0] = 'a';
  CharArray[1] = 'b';
  CharArray[2] = 'c';

  Printf("Dump Global Array:");
  Printf("Expect a: ", CharArray[0]);
  Printf("Expect a: ", CharArray[1]);
  Printf("Expect a: ", CharArray[2]);
}
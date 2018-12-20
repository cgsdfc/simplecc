char CharVar;
char CharArray[2];

void main() {
  int IntVar;
  int IntArray[2];

  IntVar = 1;
  CharVar = 'a';

  IntVar = IntVar;
  CharVar = CharVar;

  IntArray[0] = 1;
  CharArray[0] = 'a';
}

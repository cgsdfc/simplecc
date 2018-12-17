int IntVar_1, IntVar_2;
char CharVar_1, CharVar_2;

void main() {
  IntVar_1 = 1;
  IntVar_2 = 2;
  CharVar_1 = 'a';
  CharVar_2 = 'b';

  Printf("Expect 1: ", IntVar_1);
  Printf("Expect 2: ", IntVar_2);

  Printf("Expect a: ", CharVar_1);
  Printf("Expect b: ", CharVar_2);
}

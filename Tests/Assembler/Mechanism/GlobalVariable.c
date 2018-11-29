int IntVar_1, IntVar_2;
char CharVar_1, CharVar_2;

void Function {
  IntVar_1 = 1;
  IntVar_2 = 2;
  CharVar_1 = 'a';
  CharVar_2 = 'b';

  Printf("IntVar_1: ", IntVar_1);
  Printf("IntVar_2: ", IntVar_2);

  Printf("CharVar_1: ", CharVar_1);
  Printf("CharVar_2: ", CharVar_2);
}

void main() {
  Function;
}

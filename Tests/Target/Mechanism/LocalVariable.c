void TestLocalVariable {
  int IntVar_1, IntVar_2;
  char CharVar_1;

  IntVar_1 = 1;
  IntVar_2 = 2;
  CharVar_1 = 'a';

  Printf("Dump Local Variable:");
  Printf("Expect 1: ", IntVar_1);
  Printf("Expect 2: ", IntVar_2);
  Printf("Expect a: ", CharVar_1);
}

void main() {
  TestLocalVariable;
}

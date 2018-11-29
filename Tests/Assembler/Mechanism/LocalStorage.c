void Function(int I1, int I2, char C3) {
  int IntVar_1, IntVar_2;
  char CharArray[2];

  IntVar_1 = 100;
  IntVar_2 = 101;
  
  CharArray[0] = 'z';
  CharArray[1] = 'y';

  Printf("Dump Arguments:");
  Printf("I1: ", I1);
  Printf("I2: ", I2);
  Printf("C3: ", C3);

  Printf("Dump Local Variables:");
  Printf("IntVar_1: ", IntVar_1);
  Printf("IntVar_2: ", IntVar_2);

  Printf("Dump Local Array:");
  Printf("CharArray[0]: ", CharArray[0]);
  Printf("CharArray[1]: ", CharArray[1]);
}

void main() {
  Function(1, 2, 'a');
}

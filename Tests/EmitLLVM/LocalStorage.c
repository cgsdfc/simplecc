void Function(int I1, int I2, char C3) {
  int IntVar_1, IntVar_2;
  char CharArray[2];

  IntVar_1 = 100;
  IntVar_2 = 101;

  CharArray[0] = 'z';
  CharArray[1] = 'y';

  Printf("Dump Arguments:");
  Printf("Expect 1: ", I1);
  Printf("Expect 2: ", I2);
  Printf("Expect a: ", C3);

  Printf("Dump Local Variables:");
  Printf("Expect 100: ", IntVar_1);
  Printf("Expect 101: ", IntVar_2);

  Printf("Dump Local Array:");
  Printf("Expect z: ", CharArray[0]);
  Printf("Expect y: ", CharArray[1]);
}

void main() {
  Function(1, 2, 'a');
}

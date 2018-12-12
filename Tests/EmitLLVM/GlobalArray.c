int IArray[2];
char CArray[2];

void Test_IArray {
  Printf("Test_IArray:");
  Printf("Expect 0: ", IArray[0]);
  Printf("Expect 0: ", IArray[1]);

  IArray[0] = 1;
  Printf("Expect 1: ", IArray[0]);
  IArray[1] = 2;
  Printf("Expect 2: ", IArray[1]);
}


void Test_CArray {
  Printf("Test_CArray:");
  Printf("Expect 0: ", (CArray[0]));
  Printf("Expect 0: ", (CArray[1]));

  CArray[0] = 'a';
  Printf("Expect a: ", CArray[0]);
  CArray[1] = 'b';
  Printf("Expect b: ", CArray[1]);
}

void main() {
  Test_IArray;
  Test_CArray;
}

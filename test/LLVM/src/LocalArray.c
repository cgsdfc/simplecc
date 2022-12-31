void Test_IArray {
  int IArray[2];

  Printf("Test_IArray:");
  IArray[0] = 1;
  Printf("Expect 1: ", IArray[0]);
  IArray[1] = 2;
  Printf("Expect 2: ", IArray[1]);
}


void Test_CArray {
  char CArray[2];

  Printf("Test_CArray:");
  CArray[0] = 'a';
  Printf("Expect a: ", CArray[0]);
  CArray[1] = 'b';
  Printf("Expect b: ", CArray[1]);
}

void main() {
  Test_IArray;
  Test_CArray;
}

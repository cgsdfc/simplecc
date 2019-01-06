void TestArgumnentPassing(int I1, int I2, char C1, char C2) {
  Printf("Dump Arguments:");
  Printf("Expect 1: ", I1);
  Printf("Expect 2: ", I2);
  Printf("Expect a: ", C1);
  Printf("Expect b: ", C2);
}

void main() {
  TestArgumnentPassing(1, 2, 'a', 'b');
}

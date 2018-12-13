void TestCharInvolved {
  Printf("Expect 97: ", ('a'));
  Printf("Expect 194: ", 2 * 'a');
}

void TestBinOp {
  int I;
  I = 1;
  Printf("Expect 2: ", I * 2);
  Printf("Expect 2: ", I + I);
}

void main() {
  TestBinOp;
  TestCharInvolved;
}

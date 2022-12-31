int TheArray[3];

void TestNamedConst {
  const int Neg = -1;
  const int Max = 3;

  Printf(TheArray[Neg]);
  Printf(TheArray[Max]);
}

void TestUnaryOp {
  Printf(TheArray[-1]);
  Printf(TheArray[+3]);
}

void TestNum {
  Printf(TheArray[999]);
}

void TestNonChecked {
  int Var;

  Printf(TheArray[(-1)]);
  Printf(TheArray[0 - 1]);
  Var = -1;
  Printf(TheArray[Var]);
}

void TestCorrect {
  const int Idx = 1;

  Printf(TheArray[0]);
  Printf(TheArray[Idx]);
  Printf(TheArray[-0]);
  Printf(TheArray[+1]);
  Printf(TheArray[2]);
}

void main() {}

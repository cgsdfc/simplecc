const int CompareEqual = 0, CompareNotEqual = 1, CompareLess = 2,
          CompareLessEqual = 3, CompareGreater = 4, CompareGreaterEqual = 5;

void PrintCompareOp(int Op) {
  if (Op == CompareEqual)
    Printf("CompareEqual");
  else if (Op == CompareNotEqual)
    Printf("CompareNotEqual");
  else if (Op == CompareLess)
    Printf("CompareLess");
  else if (Op == CompareLessEqual)
    Printf("CompareLessEqual");
  else if (Op == CompareGreater)
    Printf("CompareGreater");
  else if (Op == CompareGreaterEqual)
    Printf("CompareGreaterEqual");
  else
    Printf("Unknown CompareOp: ", Op);
}

void main() {
  int Op;
  Op = 0;
  while (Op < 6) {
    PrintCompareOp(Op);
    Op = Op + 1;
  }
  PrintCompareOp(100);
}

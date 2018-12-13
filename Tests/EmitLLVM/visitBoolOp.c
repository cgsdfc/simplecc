void TestCharInvolved {
  char CV;
  CV = 'a';
  if ((CV));
  if (CV + 1);
  if ((CV) == ('a'));
}

void TestExprAsCond {
  int IV;
  IV = 1;

  if (IV + 1);
  if (IV * 1);
  if (1);
  if (-IV);
}

void TestRichCompareOp {
  int IV;
  IV = 1;

  if (IV > 0);
  if (IV < 0);
  if (IV == 0);
  if (IV >= 0);
  if (IV != 0);
  if (IV <= 0);
}


void main() {
  TestRichCompareOp;
  TestExprAsCond;
  TestCharInvolved;
}

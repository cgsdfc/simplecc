int IntVar;

void NoneConstants(int _) {
  printf(IntVar + IntVar);
  printf(IntVar / IntVar);
  printf(IntVar * IntVar);
  printf(IntVar - IntVar);
}

void ZeroDivisor(int _) {
  printf(IntVar / 0);
  printf(1 / 0);
}

void OneSideNoOpportunity(int _) {
  printf(1 + IntVar);
  printf(IntVar + 1 + 1);
  printf(1 / IntVar);
}

void NoOpportunity(int _) {
  NoneConstants(0);
  ZeroDivisor(0);
  OneSideNoOpportunity(0);
}

void BothConstants(int _) {
  printf(1 + 1);
  printf(1 - 1);
  printf(1 * 1);
  printf(1 / 1);
}

void OneSideConstant_Add(int _) {
  printf(0 + IntVar);
  printf(IntVar + 0);
}

void OneSideConstant_Sub(int _) {
  printf(0 - IntVar);
}

void OneSideConstant_Mult(int _) {
  printf(0 * IntVar);
  printf(1 * IntVar);
  printf(IntVar * 0);
  printf(IntVar * 1);
}

void OneSideConstant_Div(int _) {
  printf(IntVar / 1);
  printf(0 / IntVar);
}

void OneSideConstant(int _) {
  OneSideConstant_Add(0);
  OneSideConstant_Sub(0);
  OneSideConstant_Mult(0);
  OneSideConstant_Div(0);
}

void Opt(int _) {
  BothConstants(0);
  OneSideConstant(0);
}

void main() {
  Opt(0);
  NoOpportunity(0);
}
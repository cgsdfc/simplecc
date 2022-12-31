int X;

void TestOptimize(int a) {
  printf(2);
  for (X = 1; 0; X = X + 1) {
    printf(X);
  }
  printf(1);
}

void TestNeedFold(int a) {
  for (X = 1; 1 != 1; X = X + 1) {
    printf(X);
  }
}

void main() {}
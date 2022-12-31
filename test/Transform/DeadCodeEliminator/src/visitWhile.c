int X;

void deleteIfConditionIsFalse(int a) {
  while (0) {
    printf(1);
  }
}

void withinStmtList(int a) {
  printf(1);
  while (0) {
    X = 1;
  }
  printf(2);
}

void NeedConstFold(int a) {
  while (X - X) {
    printf(1);
  }
}

void NoOptimize(int a) {
  while (X) {
    X = 1;
  }
}

void main() {

}
int X;

void deleteThenBranch(int a) {
  if (0) {
    printf("Then");
  } else {
    printf("Else");
  }
  X = 1;
}

void deleteElseBranch(int a) {
  if (1) {
    printf("Then");
  } else {
    printf("Else");
  }
  X = 1;
}

void NoOptimize(int a) {
  if (X) {

  } else {

  }
}

void main() {

}

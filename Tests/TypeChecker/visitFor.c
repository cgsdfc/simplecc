void Test {}

void main() {
  int IntVar;

  for (Test = 0; Test < 3; Test = Test + 1) {
    printf("bad for");
  }

  for (IntVar = 0; IntVar < 3; IntVar = IntVar + 1) {
    printf("good for");
  }

}

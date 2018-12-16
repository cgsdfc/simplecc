int IntVar;

void Test1 {
  while (1) {
    IntVar = 1;
  }
}

void Test2 {
  while (IntVar < 1) {
    IntVar = 1;
  }
}

void main() {}

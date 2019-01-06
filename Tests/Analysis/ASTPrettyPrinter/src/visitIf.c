int IntVar;

void Test1 {
  if (1) {
    IntVar = 1;
  }
}

void Test2 {
  if (1) {
    IntVar = 1;
  } else {
    IntVar = 2;
  }
}

void Test3 {
  if (1) {} else {}
}

void Test4 {
  if (1) {}
  else {
    IntVar = 1;
  }
}

void main() {}

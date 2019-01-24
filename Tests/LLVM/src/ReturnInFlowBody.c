void Test {
  if (1) {
    return;
  }
}

void Test2 {
  while (1) {
    return;
  }
}

void Test3 {
  int I;
  for (I = 0; I < 3; I = I + 1) {
    return;
  }
}

void main() {}

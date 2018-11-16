void test {
  i = 1;
  j = 2;
  test2(1);
}

void test2(int i) {
  test(i);
  test2(1);
}

void main() {}

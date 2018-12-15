void Test(int Arg) {}

void main() {
  const int IntConst = 1;
  int IntArray[10];
  char CharVar;

  printf(IntArray);
  printf("array", IntArray);

  printf(Test(1));
  printf("void", Test(1));

  printf(CharVar);
  printf("char", CharVar);
  printf(IntArray[0]);
  printf("array[x]", IntArray[0]);
}

void main() {
  int IntArray[10];
  int IntVar;
  char CharVar;

  if (CharVar) {
    printf("type error in if");
  }

  if (CharVar == CharVar) {
    printf("type error in if");
  }

  if (IntVar == CharVar) {
    printf("type error in if");
  }

  if (IntVar) {
    printf("ok");
  }

  if (IntVar == IntVar) {
    printf("ok");
  }

}

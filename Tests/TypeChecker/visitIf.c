void main() {
  int iarr[10];
  int ivar;
  char cvar;

  if (cvar) {
    printf("type error in if");
  }

  if (cvar == cvar) {
    printf("type error in if");
  }

  if (ivar == cvar) {
    printf("type error in if");
  }

  if (ivar) {
    printf("ok");
  }

  if (ivar == ivar) {
    printf("ok");
  }

}

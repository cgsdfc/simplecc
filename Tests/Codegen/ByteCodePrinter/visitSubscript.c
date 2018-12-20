void main() {
  int iarr[2];
  int ivar;
  char carr[2];

  iarr[0] = 1;
  ivar = iarr[0];
  iarr[0] = iarr[1];
  ivar = iarr[1 + 1];
}

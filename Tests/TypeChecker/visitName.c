void main() {
  const int iconst = 1;
  const char cconst = 'a';
  int ivar;
  char cvar;
  int iarr[10];
  char carr[10];

  printf(iarr);
  printf(iarr + 1);

  iarr = 1;
  iconst = 1;

  ivar = 1;
  ivar = iconst + 1;
}

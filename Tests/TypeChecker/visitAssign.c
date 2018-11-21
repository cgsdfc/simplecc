void fun(int i) {}
char cfun {}
int ifun {}

void main() {
  const int iconst = 1;
  const char cconst = 'a';
  int ivar;
  char cvar;
  int iarr[10];
  char carr[10];

  iconst = 1;
  iarr = 1;
  fun = 1;

  ivar = iarr;
  cvar = iarr;

  ivar = cconst;
  cvar = iconst;
  iarr[0] = cconst;
  carr[0] = iconst;
  cvar = ifun;
  ivar = cfun;

  ivar = 1;
  ivar = ifun;
  ivar = iarr[0];
  iarr[0] = 1;
  iarr[0] = ivar;
  iarr[0] = ifun;
}

void vfun {}
int ifun {}
char cfun {}

void main() {
  const int iconst = 1;
  const char cconst = 'a';
  int ivar;
  char cvar;
  int iarr[2];
  char carr[2];

  iconst[0] = 1;
  ivar = iconst[0];
  vfun[0] = 1;
  ivar = vfun[0];

  ivar = iarr[cconst];
  ivar = iarr[cfun];
  ivar = iarr[vfun];
  ivar = iarr[iarr];
  ivar = iarr[cvar];
  ivar = iarr[carr[0]];

  ivar = iarr[0];
  ivar = iarr[iconst];
  ivar = iarr[('a')];
  ivar = iarr[ifun];
  ivar = iarr[iarr[0]];

}

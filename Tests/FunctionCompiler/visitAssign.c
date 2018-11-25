char cvar;
char carr[2];

void main() {
  int ivar;
  int iarr[2];

  ivar = 1;
  cvar = 'a';

  ivar = ivar;
  cvar = cvar;

  iarr[0] = 1;
  carr[0] = 'a';
}

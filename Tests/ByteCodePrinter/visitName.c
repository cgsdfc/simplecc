const char cconst = 'a';
char cvar;

void main() {
  const int iconst = 1;
  int ivar;

  printf(iconst);
  printf(cconst);

  printf(ivar);
  printf(cvar);

  ivar = 1;
  cvar = 'a';
}

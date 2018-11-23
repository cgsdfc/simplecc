void v_fun_i(int arg) {}

void main() {
  const int iconst = 1;
  int iarr[10];
  int ivar;
  char cvar;

  printf(iarr);
  printf("array", iarr);

  printf(v_fun_i(1));
  printf("void", v_fun_i(1));

  printf(cvar);
  printf("char", cvar);
  printf(iarr[0]);
  printf("array[x]", iarr[0]);
}

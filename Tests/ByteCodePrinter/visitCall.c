void vfun {}

void vfun_c(char a1) {}

void vfun_iiii(int a1, int a2, int a3, int a4) {}

void vfun_cccc(char a1, char a2, char a3, char a4) {}

char cfun {
  return('a');
}

char cfun_i(int a1) {
  return('a');
}

void main() {
  vfun;
  vfun_c('a');
  vfun_iiii(1, 2, 3, 4);
  vfun_cccc('a', 'b', 'c', 'd');

  printf(cfun);
  printf(cfun_i(1));

}

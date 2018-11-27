void vfun {
  arg = 1;
  vfun_i(1);
}

void vfun_i(int arg) {
  vfun;
}

void vfun_c(char arg) {
  vfun_c('a');
}

void main() {}

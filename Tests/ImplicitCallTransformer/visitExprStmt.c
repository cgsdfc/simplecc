int ifun {
  return(1);
}

void vfun {}

int ifun_i(int a) {
  return(a);
}

void vfun_i(int a) {}

void main() {
  ifun;
  vfun;
  ifun_i(1);
  vfun_i(1);

}

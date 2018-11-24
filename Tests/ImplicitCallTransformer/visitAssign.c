int ifun {
  return(1);
}

void main() {
  int ivar;
  int iarr[2];

  ivar = ifun;
  ivar = ifun + 1;

  iarr[0] = ifun;
  iarr[ifun] = 1;
  iarr[ifun] = ifun;

}

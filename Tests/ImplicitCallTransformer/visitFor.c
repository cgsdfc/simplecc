int ifun {
  return(1);
}

void main() {
  int ivar;
  for (ivar = ifun; ivar < ifun + 1; ivar = ivar + 1) {
    printf(ifun);
  }

}

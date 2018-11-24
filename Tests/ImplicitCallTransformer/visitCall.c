int ifun {
  return(1);
}

char cfun {
  return('c');
}

int ifun_ii(int a1, char a2) {
  return(1);
}

void main() {
  ifun_ii(ifun, cfun);
}

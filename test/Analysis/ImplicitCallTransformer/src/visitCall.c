int IntFunc {
  return(1);
}

char CharFunc {
  return('c');
}

int IntFunctII(int a1, char a2) {
  return(1);
}

void main() {
  IntFunctII(IntFunc, CharFunc);
}

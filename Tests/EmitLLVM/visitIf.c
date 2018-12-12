void Test {
  if (1) {
    return;
    if (2) return;
  }
}

void main() {
  int ivar;

  if (1) {
    ivar = 1;
  }

  if (1) {
    ivar = 1;
  }
  else {
    ivar = 2;
  }

}

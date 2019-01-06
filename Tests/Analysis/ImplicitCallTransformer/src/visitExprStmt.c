int IntFunc {
  return(1);
}

void VoidFunc {}

int IntFuncI(int a) {
  return(a);
}

void VoidFuncI(int a) {}

void main() {
  IntFunc;
  VoidFunc;
  IntFuncI(1);
  VoidFuncI(1);

}

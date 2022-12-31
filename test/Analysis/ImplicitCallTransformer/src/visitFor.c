int IntFunc {
  return(1);
}

void main() {
  int IntVar;
  for (IntVar = IntFunc; IntVar < IntFunc + 1; IntVar = IntVar + 1) {
    printf(IntFunc);
  }
}
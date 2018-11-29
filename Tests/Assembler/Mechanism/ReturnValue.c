int IntFunction_1 {
  return(1);
}

int IntFunction_2 {
  return(2);
}

void main() {
  int IntVar_1, IntVar_2;

  IntVar_1 = IntFunction_1;
  IntVar_2 = IntFunction_2;
  Printf(IntVar_1);
  Printf(IntVar_2);
}

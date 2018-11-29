int Function_1(int I1) {
  return(I1);
}

int Function_2(int I1, int I2) {
  return(Function_1(I1) + I2);
}

int Function_3(int I1, int I2, int I3) {
  return(Function_2(I1, I2) + I3);
}

int Function_4(int I1, int I2, int I3, int I4) {
  return(Function_3(I1, I2, I3) + I4);
}

void main() {
  Printf("Expect 1: ", Function_1(1));
  Printf("Expect 3: ", Function_2(1, 2));
  Printf("Expect 6: ", Function_3(1, 2, 3));
  Printf("Expect 10: ", Function_4(1, 2, 3, 4));
}

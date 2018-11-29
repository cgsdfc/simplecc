int IntFunction_Int_Int(int IntArg1, int IntArg2) {
  return(IntArg1 + IntArg2);
}

void VoidFunction_Void {
  printf("hello");
}

void main() {
  IntFunction_Int_Int(1, 2);
  VoidFunction_Void;
}

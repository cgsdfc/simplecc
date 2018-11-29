int IntFunction_Int_Int(int IntArg1, int IntArg2) {
  return(IntArg1 + IntArg2);
}

void VoidFunction_Void {
  printf("hello");
}

void main() {
  Printf("IntFunction_Int_Int");
  IntFunction_Int_Int(1, 2);
  Printf("VoidFunction_Void");
  VoidFunction_Void;
  Printf("End of call");
}
Dump Arguments:
I1: 1
I2: 2
C1: a
C2: b
End of Dump

-- program is finished running --


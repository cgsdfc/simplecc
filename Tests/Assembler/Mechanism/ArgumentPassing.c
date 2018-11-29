void Function(int I1, int I2, char C1, char C2) {
  Printf("Dump Arguments:");
  Printf("I1: ", I1);
  Printf("I2: ", I2);
  Printf("C1: ", C1);
  Printf("C2: ", C2);
  Printf("End of Dump");
}

void main() {
  Function(1, 2, 'a', 'b');
}

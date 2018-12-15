int IntFunc {
  return(1);
}

void main() {
  int IntVar;
  int IntArray[2];

  IntVar = IntFunc;
  IntVar = IntFunc + 1;

  IntArray[0] = IntFunc;
  IntArray[IntFunc] = 1;
  IntArray[IntFunc] = IntFunc;

}

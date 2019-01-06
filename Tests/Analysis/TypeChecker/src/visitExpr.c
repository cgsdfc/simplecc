void main() {
  int IntVar;
  int IntArray[10];

  IntVar = IntArray + IntVar;
  IntVar = IntVar + IntArray;
  IntVar = -IntArray;
  IntVar = +IntArray;

  IntVar = IntArray[0];
  IntVar = IntArray[IntArray[0]];
  IntVar = IntArray[IntArray[IntArray[0]]];
}

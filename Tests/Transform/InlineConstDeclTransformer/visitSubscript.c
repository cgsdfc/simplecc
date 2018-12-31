const int IC = 0;

void main() {
  int IntArray[2];
  int IntVar;

  IntArray[IC] = 1;
  IntArray[1] = IC;

  IntVar = IntArray[IC];
  IntArray[IC] = IntArray[1];
}
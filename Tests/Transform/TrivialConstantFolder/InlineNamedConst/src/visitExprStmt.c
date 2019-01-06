const char C1 = 'a', C2 = 'b', C3 = 'c', C4 = 'd';
const int I1 = 1, I2 = 2, I3 = 3, I4 = 4;

void Test1(char a1, char a2, char a3, char a4) {}

void Test2(int a1, int a2, int a3, int a4) {}

void main() {
  Test1(C1, C2, C3, C4);
  Test2(I1, I2, I3, I4);
}
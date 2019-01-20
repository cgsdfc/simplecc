void Fun1 {}
void Fun2(int A) {}
void Fun3(int A, int B) {}

void main() {
  Fun1;
  Fun2(1);
  Fun2(1 + 1);
  Fun3(1, 1);
}

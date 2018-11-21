void fun1(int a1, int a2) {

}

void fun2 {

}

void main() {
  char c;

  fun1(1);
  fun1(1, 2);
  fun1(1, 2, 3);
  fun1(fun2, 1);
  fun1(1, fun2);
  fun1(c, 1);
  fun1(1, c);
  fun1(1, (c));
}

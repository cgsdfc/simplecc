void fun(int arg) {}

void main() {
  const int const_ok = 1;
  int array[10];
  int int_ok;

  printf("function", fun);
  printf("array", array);
  printf("const", const_ok);
  printf("int", int_ok);
  printf("expr", int_ok + const_ok);
}

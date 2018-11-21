void fun {}

void main() {
  int ivar;

  for (ivar = 0; ivar < 3; ivar = ivar +  1) {
    printf("good for");
  }

  for (fun = 0; fun < 3; fun = fun + 1) {
    printf("bad for");
  }

}

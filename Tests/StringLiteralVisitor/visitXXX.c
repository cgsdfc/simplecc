void vfun {
  printf("string in <function>");
}

void main() {
  int ivar;

  printf("string");

  if (1) {
    printf("string in <if>");
  }

  for (ivar = 1; ivar < 2; ivar = ivar + 1) {
    printf("string in <for>");
  }

  while (1) {
    printf("string in <while>");
  }

}

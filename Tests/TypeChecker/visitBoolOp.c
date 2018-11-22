char c_fun1 {
  return ('a');
}

void main() {
  const int iconst = 1;
  const char cconst = 'a';
  char carr[1];

  if (iconst > cconst) {
    printf("type of condition must be int");
  }

  if (cconst != carr[0]) {
    printf("type of condition must be int");
  }

  if (iconst > carr[0]) {
    printf("type of condition must be int");
  }

  if ('a') {
    printf("type of condition must be int");
  }

  if (c_fun1) {
    printf("type of condition must be int");
  }

  if (carr[0]) {
    printf("type of condition must be int");
  }

  if (1) {
    printf("ok");
  }

  if (('a')) {
    printf("ok");
  }

  if (iconst > cconst + 1) {
    printf("ok");
  }

}

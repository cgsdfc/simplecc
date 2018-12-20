char CharFunc {
  return ('a');
}

void main() {
  const int IntConst = 1;
  const char CharConst = 'a';
  char CharArray[1];

  if (IntConst > CharConst) {
    printf("type of condition must be int");
  }

  if (CharConst != CharArray[0]) {
    printf("type of condition must be int");
  }

  if (IntConst > CharArray[0]) {
    printf("type of condition must be int");
  }

  if ('a') {
    printf("type of condition must be int");
  }

  if (CharFunc) {
    printf("type of condition must be int");
  }

  if (CharArray[0]) {
    printf("type of condition must be int");
  }

  if (1) {
    printf("ok");
  }

  if (('a')) {
    printf("ok");
  }

  if (IntConst > CharConst + 1) {
    printf("ok");
  }

  if (1 + 1) {
    printf("ok");
  }

}

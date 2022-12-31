const int IntConst = 1;
const char CharConst = 'a';
char CharArray[1];

char CharFun(int a) {
  return ('a');
}

void HasCompareOp_BAD(int a) {
  if (IntConst > CharConst);
  if (CharConst != CharArray[0]);
  if (IntConst > CharArray[0]);
}

void NoCompareOp_BAD(int a) {
  if ('a');
  if (CharFun(0));
  if (CharArray[0]);
}

void HasCompareOp_OK(int a) {
  if (IntConst > CharConst + 1);
}

void NoCompareOp_OK(int a) {
  if (1);
  if (('a'));
  if (1 + 1);
}

void main() {}
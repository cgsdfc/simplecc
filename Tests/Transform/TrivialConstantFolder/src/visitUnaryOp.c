int X;

void StripUAdd(int _) {
  printf(+1);
  printf(+X);
  printf(++X);
  printf(++1);
}

void ConstUSub(int _) {
  printf(-1);
  printf(-2);
  printf(--1);
  printf(--2);
}

void UUSub(int _) {
  printf(--X);
  printf(-- -X);
  printf(-- --X);
}

void MixUAddAndUSub(int _) {
  printf(-- +X);
  printf(++ -X);
  printf(- + -X);
  printf(+ - +X);
}

void NoOpportunity(int _) {
  printf(-X);
}

void main() {
  StripUAdd(0);
  ConstUSub(0);
  UUSub(0);
  MixUAddAndUSub(0);
  NoOpportunity(0);
}
void ConstConstRedefined {
  const int iconst = 1;
  const int iconst = 2;
}

void ArgArgRedefined(int arg, int arg) {}

void ArgLocalVarRedefined(int arg) {
  int arg;
}

void ArgLocalConstRedefined(char arg) {
  const char arg = 'c';
}

void main() {}
void GlobalFunction {}

void ArgShallow(int GlobalFunction) {}

void LocalVarShallow(int arg) {
  int GlobalFunction;
}

void LocalConstShallow(int a1, int a2) {
  const int GlobalFunction = 1;
}

void main() {}
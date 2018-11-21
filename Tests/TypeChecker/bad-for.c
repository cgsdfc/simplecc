void fun {}

void main() {
  int iok;

  for (fun = 0; iok < 0; iok = iok + 1) {}
  for (iok = 0; fun < 0; iok = iok + 1) {}
  for (iok = 0; iok < 0; fun = iok + 1) {}
  for (iok = 0; iok < 0; iok = iok + 1) {
    fun = 0;
  }

}

void read_and_sum(int n_args) {
  int i, t, s;

  s = 0;
  for (i = 0; i < n_args; i = i + 1) {
    scanf(t);
    s = s + t;
  }
  return (s);
}

void main() {
  printf("read_and_sum(4):", read_and_sum(4));
}

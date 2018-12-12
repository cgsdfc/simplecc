int IV;
char CV;

void main() {
  if (IV == 0) Printf("IV == 0");
  if ((CV) == 0) Printf("CV == 0");

  IV = 1; Printf("Expect 1: ", IV);
  CV = 'a'; Printf("Expect a: ", CV);
}

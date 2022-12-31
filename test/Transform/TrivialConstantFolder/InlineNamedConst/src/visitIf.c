const int IC1 = 1;
const int IC2 = 2;
int IntVar;

void Test1 {
  if (IC1) {
    IntVar = IC2;
  }
}

void Test2 {
  if (IC1) {
    IntVar = IC1;
  }
  else {
    IntVar = IC2;
  }
}

void main() {}
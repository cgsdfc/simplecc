int RecursiveFactorial(int Order) {

  if (Order < 0) {
    printf("Invalid Order: ", Order);
    return(-1);
  }

  if (Order == 0) return(1);

  return(Order * RecursiveFactorial(Order - 1));
}

void main() {
  Printf("Expect -1");
  RecursiveFactorial(-1);
  Printf("Expect 1: ", RecursiveFactorial(0));
  Printf("Expect 1: ", RecursiveFactorial(1));
  Printf("Expect 2: ", RecursiveFactorial(2));
  Printf("Expect 6: ", RecursiveFactorial(3));
  Printf("Expect 24: ", RecursiveFactorial(4));
  Printf("Expect 120: ", RecursiveFactorial(5));
  Printf("Expect 720: ", RecursiveFactorial(6));
}

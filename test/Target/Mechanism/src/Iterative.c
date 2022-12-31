int IterativeFactorial(int Order) {
  int Product;
  int I;

  if (Order < 0) {
    printf("Invalid Order: ", Order);
    return(-1);
  }

  if (Order == 0) return(1);

  Product = 1;
  I = 1;
  while (I <= Order) {
    Product = Product * I;
    I = I + 1;
  }
  return(Product);
}

void main() {
  Printf("Expect -1: ");
  IterativeFactorial(-1);

  Printf("Expect 1: ", IterativeFactorial(0));
  Printf("Expect 1: ", IterativeFactorial(1));
  Printf("Expect 2: ", IterativeFactorial(2));
  Printf("Expect 6: ", IterativeFactorial(3));
  Printf("Expect 24: ", IterativeFactorial(4));
  Printf("Expect 120: ", IterativeFactorial(5));
  Printf("Expect 720: ", IterativeFactorial(6));
}

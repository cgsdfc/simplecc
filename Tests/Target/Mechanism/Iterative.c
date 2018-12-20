int IterativeFactoral(int Order) {
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
  IterativeFactoral(-1);

  Printf("Expect 1: ", IterativeFactoral(0));
  Printf("Expect 1: ", IterativeFactoral(1));
  Printf("Expect 2: ", IterativeFactoral(2));
  Printf("Expect 6: ", IterativeFactoral(3));
  Printf("Expect 24: ", IterativeFactoral(4));
  Printf("Expect 120: ", IterativeFactoral(5));
  Printf("Expect 720: ", IterativeFactoral(6));
}

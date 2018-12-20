int RecursiveFactoral(int Order) {

  if (Order < 0) {
    printf("Invalid Order: ", Order);
    return(-1);
  }

  if (Order == 0) return(1);

  return(Order * RecursiveFactoral(Order - 1));
}

void main() {
  Printf("Expect -1");
  RecursiveFactoral(-1);
  Printf("Expect 1: ", RecursiveFactoral(0));
  Printf("Expect 1: ", RecursiveFactoral(1));
  Printf("Expect 2: ", RecursiveFactoral(2));
  Printf("Expect 6: ", RecursiveFactoral(3));
  Printf("Expect 24: ", RecursiveFactoral(4));
  Printf("Expect 120: ", RecursiveFactoral(5));
  Printf("Expect 720: ", RecursiveFactoral(6));
}

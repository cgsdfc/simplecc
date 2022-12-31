int X;
int Y;

void BothSizeConstOptimize(int a) {
  if (1 == 1);
  if (1 != 1);
  if (1 > 1);
  if (1 < 1);
  if (1 >= 2);
  if (1 <= 2);
}

void NonCompareOpOptimize(int a) {
  if (1);
  if (1 + 1);
  if (-1);
  if (0);
}

void NonCompareOpNoOptimize(int a) {
  if (X);
  if (X + Y);
  if (-X);
}

void BothSideTheSameOptimize(int a) {
  if (X == X);
  if (X != X);
  if (X > X);
  if (X < X);
  if (X <= X);
  if (X >= X);
}

void CompareOpNoOptimize(int a) {
  if (X > 1);
  if (X > Y);
}

void main() {
  
}
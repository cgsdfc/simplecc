int X;

void InsideFuncDef(int a) {
  X = 1;
  X = 2;
  return;
  X = 3;
}

void InsideIfStmt(int a) {
  if (X) {
    return;
    X = 1;
  } else {
    return;
    X = 2;
  }
}

void InsideForStmt(int a) {
  for (X = 1; X < 1; X = X + 1) {
    return;
    X = 1;
  }
}

void InsideWhileStmt(int a) {
  while (X) {
    return;
    X = 1;
  }
}

int InsideStmtList(int a) {
  {
    return (1);
    return (2);
    return (3);
    return (4);
  }
}

void main() {}
int X;

void StmtsAfterReturn(int a) {
  X = 1;
  X = 2;
  return;
  X = 3;
}

void InIfStmt(int a) {
  if (X) {
    return;
    X = 1;
  } else {
    return;
    X = 2;
  }
}

void InForStmt(int a) {
  for (X = 1; X < 1; X = X + 1) {
    return;
    X = 1;
  }
}

void InWhileStmt(int a) {
  while (X) {
    return;
    X = 1;
  }
}

int InStmtList(int a) {
  {
    return (1);
    return (2);
    return (3);
    return (4);
  }
}

void main() {}
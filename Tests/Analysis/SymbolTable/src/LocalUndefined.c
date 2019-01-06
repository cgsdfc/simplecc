void UseUndefinedName{
    arg = 1;
}

void DefineEarlier{
    DefineLater(1);
}

void DefineLater(int arg) {
  DefineEarlier;
}

void CallItself(char arg) {
  CallItself('a');
}

void main() {}
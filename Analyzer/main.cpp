#include "parser.h"
#include "cst.h"

int main(int argc, char **argv) {
  TokenBuffer tokens;
  if (argc == 1) {
    Tokenize(std::cin, tokens);
  } else if (argc == 2) {
    std::ifstream ifs(argv[1]);
    if (ifs.fail()) {
      fprintf(stderr, "file %s not exist\n", argv[1]);
      exit(1);
    }
    Tokenize(ifs, tokens);
  } else {
    fputs("Usage: parser [file]\n", stderr);
    exit(1);
  }

  Node *root = ParseTokens(tokens);
  AST *node = NodeToAst(root);
  std::cout << *node << "\n";

  delete root;
  delete node;
  for (auto token: tokens) {
    delete token;
  }
  return 0;
}

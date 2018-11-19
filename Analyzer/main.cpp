#include "parser.h"
#include "cst.h"
#include "validate.h"
#include "symtable.h"
#include "error.h"

void TestSymbolTable(SymbolTable &symtable);

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

  Node *cst_node = ParseTokens(tokens);
  if (!cst_node)
    return 1;
  Program *ast_node = NodeToAst(cst_node);
  if (!ast_node)
    return 1;
  if (!ValidateSyntax(ast_node))
    return 1;

  SymbolTable symtable;
  if (!BuildSymbolTable(ast_node, symtable))
    return 1;

  std::cout << *ast_node << "\n";
  /* std::cout << symbolTable << "\n"; */
  /* TestSymbolTable(symtable); */

  delete ast_node;
  delete cst_node;
  for (auto token: tokens) {
    delete token;
  }
  return 0;
}

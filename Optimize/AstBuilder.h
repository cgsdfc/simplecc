#ifndef AST_BUILDER_H
#define AST_BUILDER_H

namespace simplecompiler {
class Program;
class Node;
Program *NodeToAst(const Node *node);
} // namespace simplecompiler

#endif

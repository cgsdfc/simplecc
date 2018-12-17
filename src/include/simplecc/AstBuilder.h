#ifndef AST_BUILDER_H
#define AST_BUILDER_H

namespace simplecc {
class Program;
class Node;

Program *BuildAstFromNode(const Node *node);
} // namespace simplecc

#endif

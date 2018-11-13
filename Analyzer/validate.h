#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "AST.h"

/* template <typename Derived> */
/* class VisitorBase { */
/* public: */
/*   template <typename R, typename... Args> */
/*     R visit(Decl *decl) { */
/*       if (auto x = subclass_cast<ConstDecl>(decl)) */
/*         return visit(x); */
/*       if (auto x = subclass_cast<VarDecl>(decl)) */
/*         return visit(x); */
/*       if (auto x = subclass_cast<FuncDef>(decl)) */
/*         return visit(x); */
/*     } */
/* }; */

bool ValidateSyntax(Program *node);
#endif

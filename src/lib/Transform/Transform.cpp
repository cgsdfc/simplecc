#include "simplecc/Transform/Transform.h"
#include "simplecc/Transform/TrivialConstantFolder.h"
#include "simplecc/Transform/DeadCodeEliminator.h"

namespace simplecc {
void TransformProgram(Program *P, SymbolTable &S) {
  TrivialConstantFolder().Transform(P, S);
  DeadCodeEliminator().Transform(P);
}

} // namespace simplecc
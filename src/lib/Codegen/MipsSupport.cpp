#include "simplecc/Codegen/MipsSupport.h"

using namespace simplecc;

void AsciizLabel::FormatImpl(std::ostream &O) const {
  O << "string_" << StringID;
}

void JumpTargetLabel::FormatImpl(std::ostream &O) const {
  O << ParentName << "_label_" << Target;
}

void ReturnLabel::FormatImpl(std::ostream &O) const {
  O << ParentName << "_return";
}

void GlobalLabel::FormatImpl(std::ostream &O) const { O << Name; }

void EscapedString::Format(std::ostream &O) const {
  for (char C : Str) {
    O << C;
    if (C == '\\')
      O << C;
  }
}
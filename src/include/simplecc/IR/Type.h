#ifndef SIMPLECC_IR_TYPE_H
#define SIMPLECC_IR_TYPE_H

namespace simplecc {
class IRContext;

class Type {
public:
  enum TypeKind {
    IntTypeKind,
    PointerTypeKind,
    VoidTypeKind,
    FunctionTypeKind,
    LabelTypeKind,
  };

  // Primitive types.
  static Type *getVoidTy(IRContext &Context);
  static Type *getLabelTy(IRContext &Context);
  static Type *getIntTy(IRContext &Context);
  static Type *getPointerTy(IRContext &Context);

protected:
  friend class IRContext;
  TypeKind TyKind;
  Type(TypeKind Kind) : TyKind(Kind) {}
};


class FunctionType : public Type {
  Type *ReturnType;

};

}

#endif //SIMPLECC_IR_TYPE_H

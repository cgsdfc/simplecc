#include "AST.h"
#include <unordered_map>

class Type {
public:
  int subclass_tag;

  Type(int subclass_tag): subclass_tag(subclass_tag) {}
  enum { Constant, Variable, Array, Function, };
  virtual ~Type() = 0;
};

Type::~Type() {}

class Constant: public Type {
public:
  BasicTypeKind type;

  Constant(BasicTypeKind type): Type(Type::Constant), type(type) {}
  ~Constant() {}
};

class Variable: public Type {
public:
  BasicTypeKind type;

  Variable(BasicTypeKind type): Type(Type::Variable), type(type) {}
  ~Variable() override {}
};

class Array: public Type {
public:
  BasicTypeKind elemtype;
  int size;
  Array(BasicTypeKind elemtype, int size): Type(Type::Array), elemtype(elemtype), size(size) {}
  ~Array() override {}
};

class Function: public Type {
public:
  BasicTypeKind return_type;
  std::vector<BasicTypeKind> args;

  Function(BasicTypeKind return_type, const std::vector<BasicTypeKind> &args):
    Type(Type::Function), return_type(return_type), args(args) {}
  ~Function() override {}
};

enum class Scope { Global, Local };

class Entry {
public:
  Type *type;
  Scope scope;
  Location location;
  String name;

  Entry(): type(nullptr) {}
  Entry(Type *type, Scope scope, const Location &location, const String &name):
    type(type), scope(scope), location(location), name(name) {}

  // avoid double free
  Entry(Entry &&entry):
    Entry(entry.type, entry.scope, entry.location, entry.name) {
      entry.type = nullptr;
    }

  ~Entry() { delete type; }
};

using TableType = std::unordered_map<String, Entry>;
using NestedTableType = std::unordered_map<String, TableType>;

class SymbolTable {
public:

  TableType global;
  NestedTableType locals;

  SymbolTable(): global(), locals() {}
};

bool BuildSymbolTable(Program *prog, SymbolTable &table);

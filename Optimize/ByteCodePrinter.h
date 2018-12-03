#ifndef BYTE_CODE_PRINTER_H
#define BYTE_CODE_PRINTER_H
#include <iostream>

namespace simplecompiler {
class Program;
void PrintByteCode(Program *prog, std::ostream &os);
} // namespace simplecompiler
#endif

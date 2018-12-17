#ifndef BYTE_CODE_PRINTER_H
#define BYTE_CODE_PRINTER_H
#include <iostream>

namespace simplecc {
class Program;
void PrintByteCode(Program *P, std::ostream &O);
} // namespace simplecc
#endif

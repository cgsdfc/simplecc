
   1: "string "
   0: "string"

test1:
SymbolEntry(charvar, Variable, Local, Location(2, 7))
0   LOAD_STRING              0         
1   PRINT_STRING             0         
2   PRINT_NEWLINE            0         
3   LOAD_CONST               1         
4   PRINT_INTEGER            0         
5   PRINT_NEWLINE            0         
6   LOAD_CONST               97        
7   PRINT_CHARACTER          0         
8   PRINT_NEWLINE            0         
9   LOAD_LOCAL               charvar             
10  PRINT_CHARACTER          0         
11  PRINT_NEWLINE            0         
12  RETURN_NONE              

test2:
SymbolEntry(charvar, Variable, Local, Location(11, 7))
SymbolEntry(intvar, Variable, Local, Location(12, 6))
0   LOAD_STRING              1         
1   PRINT_STRING             0         
2   LOAD_LOCAL               charvar             
3   PRINT_CHARACTER          0         
4   PRINT_NEWLINE            0         
5   LOAD_STRING              1         
6   PRINT_STRING             0         
7   LOAD_LOCAL               intvar              
8   PRINT_INTEGER            0         
9   PRINT_NEWLINE            0         
10  RETURN_NONE              

main:
0   RETURN_NONE              



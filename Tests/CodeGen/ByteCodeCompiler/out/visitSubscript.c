

main:
SymbolEntry(intarray, Array, Local, Location(2, 6))
SymbolEntry(intvar, Variable, Local, Location(3, 6))
SymbolEntry(chararray, Array, Local, Location(4, 7))
0   LOAD_CONST               1         
1   LOAD_LOCAL               intarray            
2   LOAD_CONST               0         
3   STORE_SUBSCR             
4   LOAD_LOCAL               intarray            
5   LOAD_CONST               0         
6   BINARY_SUBSCR            
7   STORE_LOCAL              intvar              
8   LOAD_LOCAL               intarray            
9   LOAD_CONST               1         
10  BINARY_SUBSCR            
11  LOAD_LOCAL               intarray            
12  LOAD_CONST               0         
13  STORE_SUBSCR             
14  LOAD_LOCAL               chararray           
15  LOAD_CONST               1         
16  BINARY_SUBSCR            
17  PRINT_CHARACTER          0         
18  PRINT_NEWLINE            0         
19  RETURN_NONE              



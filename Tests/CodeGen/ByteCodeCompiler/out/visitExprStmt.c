

test1:
0   RETURN_NONE              

test2:
SymbolEntry(a1, Variable, Local, Location(3, 11))
0   RETURN_NONE              

test3:
SymbolEntry(a1, Variable, Local, Location(5, 11))
SymbolEntry(a2, Variable, Local, Location(5, 19))
SymbolEntry(a3, Variable, Local, Location(5, 27))
SymbolEntry(a4, Variable, Local, Location(5, 35))
0   RETURN_NONE              

test4:
SymbolEntry(a1, Variable, Local, Location(7, 11))
SymbolEntry(a2, Variable, Local, Location(7, 20))
SymbolEntry(a3, Variable, Local, Location(7, 29))
SymbolEntry(a4, Variable, Local, Location(7, 38))
0   RETURN_NONE              

main:
0   CALL_FUNCTION            0         test1               
1   POP_TOP                  
2   LOAD_CONST               97        
3   CALL_FUNCTION            1         test2               
4   POP_TOP                  
5   LOAD_CONST               1         
6   LOAD_CONST               2         
7   LOAD_CONST               3         
8   LOAD_CONST               4         
9   CALL_FUNCTION            4         test3               
10  POP_TOP                  
11  LOAD_CONST               97        
12  LOAD_CONST               98        
13  LOAD_CONST               99        
14  LOAD_CONST               100       
15  CALL_FUNCTION            4         test4               
16  POP_TOP                  
17  RETURN_NONE              



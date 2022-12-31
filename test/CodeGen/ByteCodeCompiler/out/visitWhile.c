SymbolEntry(intvar, Variable, Global, Location(1, 0))


test1:
0   LOAD_CONST               1         
1   JUMP_IF_FALSE            5         
2   LOAD_CONST               1         
3   STORE_GLOBAL             intvar              
4   JUMP_FORWARD             0         
5   RETURN_NONE              

test2:
0   LOAD_GLOBAL              intvar              
1   LOAD_CONST               1         
2   JUMP_IF_GREATER_EQUAL    6         
3   LOAD_CONST               1         
4   STORE_GLOBAL             intvar              
5   JUMP_FORWARD             0         
6   RETURN_NONE              

main:
0   RETURN_NONE              



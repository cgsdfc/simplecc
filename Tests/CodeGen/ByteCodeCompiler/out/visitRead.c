SymbolEntry(intvar, Variable, Global, Location(1, 0))
SymbolEntry(charvar, Variable, Global, Location(2, 0))


testglobal:
0   READ_CHARACTER           
1   STORE_GLOBAL             charvar             
2   READ_INTEGER             
3   STORE_GLOBAL             intvar              
4   RETURN_NONE              

testlocal:
SymbolEntry(intvar, Variable, Local, Location(10, 6))
SymbolEntry(charvar, Variable, Local, Location(11, 7))
0   READ_INTEGER             
1   STORE_LOCAL              intvar              
2   READ_CHARACTER           
3   STORE_LOCAL              charvar             
4   RETURN_NONE              

main:
0   RETURN_NONE              



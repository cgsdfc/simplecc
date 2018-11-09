NAME = 0
OP = 1
ERRORTOKEN = 2
ENDMARKER = 3
CHAR = 4
NUMBER = 5
STRING = 6
program = 256
arglist = 257
compound_stmt = 258
condition = 259
const_decl = 260
const_item = 261
decl_trailer = 262
declaration = 263
expr = 264
factor = 265
factor_trailer = 266
for_stmt = 267
if_stmt = 268
integer = 269
paralist = 270
read_stmt = 271
return_stmt = 272
stmt = 273
subscript2 = 274
term = 275
type_name = 276
var_decl = 277
var_item = 278
while_stmt = 279
write_stmt = 280

tok_name = {
    0: 'NAME',
    1: 'OP',
    2: 'ERRORTOKEN',
    3: 'ENDMARKER',
    4: 'CHAR',
    5: 'NUMBER',
    6: 'STRING',
    256: 'program',
    257: 'arglist',
    258: 'compound_stmt',
    259: 'condition',
    260: 'const_decl',
    261: 'const_item',
    262: 'decl_trailer',
    263: 'declaration',
    264: 'expr',
    265: 'factor',
    266: 'factor_trailer',
    267: 'for_stmt',
    268: 'if_stmt',
    269: 'integer',
    270: 'paralist',
    271: 'read_stmt',
    272: 'return_stmt',
    273: 'stmt',
    274: 'subscript2',
    275: 'term',
    276: 'type_name',
    277: 'var_decl',
    278: 'var_item',
    279: 'while_stmt',
    280: 'write_stmt',
}

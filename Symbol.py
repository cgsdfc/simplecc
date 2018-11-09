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
konstant = 270
paralist = 271
read_stmt = 272
return_stmt = 273
stmt = 274
subscript = 275
subscript2 = 276
term = 277
type_name = 278
var_decl = 279
var_item = 280
while_stmt = 281
write_stmt = 282

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
    270: 'konstant',
    271: 'paralist',
    272: 'read_stmt',
    273: 'return_stmt',
    274: 'stmt',
    275: 'subscript',
    276: 'subscript2',
    277: 'term',
    278: 'type_name',
    279: 'var_decl',
    280: 'var_item',
    281: 'while_stmt',
    282: 'write_stmt',
}

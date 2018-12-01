NAME = 0
OP = 1
ERRORTOKEN = 2
ENDMARKER = 3
CHAR = 4
NUMBER = 5
STRING = 6
program = 256
arglist = 257
atom = 258
atom_trailer = 259
compound_stmt = 260
condition = 261
const_decl = 262
const_item = 263
decl_trailer = 264
declaration = 265
expr = 266
factor = 267
flow_stmt = 268
for_stmt = 269
if_stmt = 270
integer = 271
paralist = 272
read_stmt = 273
return_stmt = 274
stmt = 275
stmt_trailer = 276
subscript2 = 277
term = 278
type_name = 279
var_decl = 280
var_item = 281
while_stmt = 282
write_stmt = 283

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
    258: 'atom',
    259: 'atom_trailer',
    260: 'compound_stmt',
    261: 'condition',
    262: 'const_decl',
    263: 'const_item',
    264: 'decl_trailer',
    265: 'declaration',
    266: 'expr',
    267: 'factor',
    268: 'flow_stmt',
    269: 'for_stmt',
    270: 'if_stmt',
    271: 'integer',
    272: 'paralist',
    273: 'read_stmt',
    274: 'return_stmt',
    275: 'stmt',
    276: 'stmt_trailer',
    277: 'subscript2',
    278: 'term',
    279: 'type_name',
    280: 'var_decl',
    281: 'var_item',
    282: 'while_stmt',
    283: 'write_stmt',
}

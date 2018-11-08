NAME = 0
OP = 1
ERRORTOKEN = 2
ENDMARKER = 3
CHAR = 4
NUMBER = 5
STRING = 6
program = 256
arglist = 257
case_stmt = 258
compound_stmt = 259
condition = 260
const_decl = 261
const_item = 262
decl_trailer = 263
declaration = 264
default_stmt = 265
expr = 266
factor = 267
for_stmt = 268
if_stmt = 269
integer = 270
konstant = 271
paralist = 272
read_stmt = 273
relation_op = 274
return_stmt = 275
stmt = 276
stmt_trailer = 277
subscript = 278
subscript2 = 279
switch_stmt = 280
term = 281
trailer = 282
type_name = 283
var_decl = 284
var_item = 285
write_stmt = 286

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
    258: 'case_stmt',
    259: 'compound_stmt',
    260: 'condition',
    261: 'const_decl',
    262: 'const_item',
    263: 'decl_trailer',
    264: 'declaration',
    265: 'default_stmt',
    266: 'expr',
    267: 'factor',
    268: 'for_stmt',
    269: 'if_stmt',
    270: 'integer',
    271: 'konstant',
    272: 'paralist',
    273: 'read_stmt',
    274: 'relation_op',
    275: 'return_stmt',
    276: 'stmt',
    277: 'stmt_trailer',
    278: 'subscript',
    279: 'subscript2',
    280: 'switch_stmt',
    281: 'term',
    282: 'trailer',
    283: 'type_name',
    284: 'var_decl',
    285: 'var_item',
    286: 'write_stmt',
}

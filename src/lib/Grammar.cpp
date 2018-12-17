
#include "Grammar.h"

const char *TokenNames[7] = {"NAME", "OP",     "ERRORTOKEN", "ENDMARKER",
                             "CHAR", "NUMBER", "STRING"};

const char *SymbolNames[28] = {
    "program",      "arglist",    "atom",       "atom_trailer", "compound_stmt",
    "condition",    "const_decl", "const_item", "decl_trailer", "declaration",
    "expr",         "factor",     "flow_stmt",  "for_stmt",     "if_stmt",
    "integer",      "paralist",   "read_stmt",  "return_stmt",  "stmt",
    "stmt_trailer", "subscript2", "term",       "type_name",    "var_decl",
    "var_item",     "while_stmt", "write_stmt"};

static Label labels[64] = {
    {0, "EMPTY"}, {3, 0},       {262, 0},    {265, 0},      {0, "char"},
    {0, "const"}, {0, "int"},   {0, "void"}, {1, "("},      {266, 0},
    {1, ")"},     {1, ","},     {4, 0},      {0, 0},        {5, 0},
    {259, 0},     {1, "["},     {257, 0},    {1, "]"},      {1, "{"},
    {1, "}"},     {275, 0},     {280, 0},    {1, "!="},     {1, "<"},
    {1, "<="},    {1, "=="},    {1, ">"},    {1, ">="},     {1, "+"},
    {1, "-"},     {279, 0},     {263, 0},    {1, ";"},      {1, "="},
    {271, 0},     {260, 0},     {272, 0},    {277, 0},      {281, 0},
    {0, "main"},  {264, 0},     {278, 0},    {258, 0},      {267, 0},
    {269, 0},     {270, 0},     {273, 0},    {274, 0},      {282, 0},
    {283, 0},     {0, "for"},   {0, "if"},   {0, "printf"}, {0, "return"},
    {0, "scanf"}, {0, "while"}, {261, 0},    {0, "else"},   {268, 0},
    {276, 0},     {1, "*"},     {1, "/"},    {6, 0}};

static Arc arcs_0_0[3] = {{1, 1}, {2, 0}, {3, 2}};

static Arc arcs_0_1[1] = {{0, 1}};

static Arc arcs_0_2[2] = {{1, 1}, {3, 2}};

static int first_0[5] = {1, 4, 5, 6, 7};

static DFAState states_0[3] = {
    {3, arcs_0_0, 0}, {1, arcs_0_1, 1}, {2, arcs_0_2, 0}};

static DFA dfa_0 = {"program", 3, states_0, 5, first_0};

static Arc arcs_1_0[1] = {{8, 1}};

static Arc arcs_1_1[1] = {{9, 2}};

static Arc arcs_1_2[2] = {{10, 3}, {11, 1}};

static Arc arcs_1_3[1] = {{0, 3}};

static int first_1[1] = {8};

static DFAState states_1[4] = {
    {1, arcs_1_0, 0}, {1, arcs_1_1, 0}, {2, arcs_1_2, 0}, {1, arcs_1_3, 1}};

static DFA dfa_1 = {"arglist", 4, states_1, 1, first_1};

static Arc arcs_2_0[4] = {{8, 1}, {12, 2}, {13, 3}, {14, 2}};

static Arc arcs_2_1[1] = {{9, 4}};

static Arc arcs_2_2[1] = {{0, 2}};

static Arc arcs_2_3[2] = {{15, 2}, {0, 3}};

static Arc arcs_2_4[1] = {{10, 2}};

static int first_2[4] = {8, 12, 13, 14};

static DFAState states_2[5] = {{4, arcs_2_0, 0},
                               {1, arcs_2_1, 0},
                               {1, arcs_2_2, 1},
                               {2, arcs_2_3, 1},
                               {1, arcs_2_4, 0}};

static DFA dfa_2 = {"atom", 5, states_2, 4, first_2};

static Arc arcs_3_0[2] = {{16, 1}, {17, 2}};

static Arc arcs_3_1[1] = {{9, 3}};

static Arc arcs_3_2[1] = {{0, 2}};

static Arc arcs_3_3[1] = {{18, 2}};

static int first_3[2] = {8, 16};

static DFAState states_3[4] = {
    {2, arcs_3_0, 0}, {1, arcs_3_1, 0}, {1, arcs_3_2, 1}, {1, arcs_3_3, 0}};

static DFA dfa_3 = {"atom_trailer", 4, states_3, 2, first_3};

static Arc arcs_4_0[1] = {{19, 1}};

static Arc arcs_4_1[4] = {{20, 2}, {2, 1}, {21, 3}, {22, 4}};

static Arc arcs_4_2[1] = {{0, 2}};

static Arc arcs_4_3[2] = {{20, 2}, {21, 3}};

static Arc arcs_4_4[3] = {{20, 2}, {21, 3}, {22, 4}};

static int first_4[1] = {19};

static DFAState states_4[5] = {{1, arcs_4_0, 0},
                               {4, arcs_4_1, 0},
                               {1, arcs_4_2, 1},
                               {2, arcs_4_3, 0},
                               {3, arcs_4_4, 0}};

static DFA dfa_4 = {"compound_stmt", 5, states_4, 1, first_4};

static Arc arcs_5_0[1] = {{9, 1}};

static Arc arcs_5_1[7] = {{23, 2}, {24, 2}, {25, 2}, {26, 2},
                          {27, 2}, {28, 2}, {0, 1}};

static Arc arcs_5_2[1] = {{9, 3}};

static Arc arcs_5_3[1] = {{0, 3}};

static int first_5[6] = {8, 12, 13, 14, 29, 30};

static DFAState states_5[4] = {
    {1, arcs_5_0, 0}, {7, arcs_5_1, 1}, {1, arcs_5_2, 0}, {1, arcs_5_3, 1}};

static DFA dfa_5 = {"condition", 4, states_5, 6, first_5};

static Arc arcs_6_0[1] = {{5, 1}};

static Arc arcs_6_1[1] = {{31, 2}};

static Arc arcs_6_2[1] = {{32, 3}};

static Arc arcs_6_3[2] = {{11, 2}, {33, 4}};

static Arc arcs_6_4[1] = {{0, 4}};

static int first_6[1] = {5};

static DFAState states_6[5] = {{1, arcs_6_0, 0},
                               {1, arcs_6_1, 0},
                               {1, arcs_6_2, 0},
                               {2, arcs_6_3, 0},
                               {1, arcs_6_4, 1}};

static DFA dfa_6 = {"const_decl", 5, states_6, 1, first_6};

static Arc arcs_7_0[1] = {{13, 1}};

static Arc arcs_7_1[1] = {{34, 2}};

static Arc arcs_7_2[2] = {{12, 3}, {35, 3}};

static Arc arcs_7_3[1] = {{0, 3}};

static int first_7[1] = {13};

static DFAState states_7[4] = {
    {1, arcs_7_0, 0}, {1, arcs_7_1, 0}, {2, arcs_7_2, 0}, {1, arcs_7_3, 1}};

static DFA dfa_7 = {"const_item", 4, states_7, 1, first_7};

static Arc arcs_8_0[5] = {{11, 1}, {33, 2}, {36, 2}, {37, 3}, {38, 4}};

static Arc arcs_8_1[1] = {{39, 4}};

static Arc arcs_8_2[1] = {{0, 2}};

static Arc arcs_8_3[1] = {{36, 2}};

static Arc arcs_8_4[2] = {{11, 1}, {33, 2}};

static int first_8[5] = {33, 8, 11, 16, 19};

static DFAState states_8[5] = {{5, arcs_8_0, 0},
                               {1, arcs_8_1, 0},
                               {1, arcs_8_2, 1},
                               {1, arcs_8_3, 0},
                               {2, arcs_8_4, 0}};

static DFA dfa_8 = {"decl_trailer", 5, states_8, 5, first_8};

static Arc arcs_9_0[1] = {{31, 1}};

static Arc arcs_9_1[2] = {{40, 2}, {13, 3}};

static Arc arcs_9_2[1] = {{8, 4}};

static Arc arcs_9_3[1] = {{41, 5}};

static Arc arcs_9_4[1] = {{10, 6}};

static Arc arcs_9_5[1] = {{0, 5}};

static Arc arcs_9_6[1] = {{36, 5}};

static int first_9[3] = {4, 6, 7};

static DFAState states_9[7] = {
    {1, arcs_9_0, 0}, {2, arcs_9_1, 0}, {1, arcs_9_2, 0}, {1, arcs_9_3, 0},
    {1, arcs_9_4, 0}, {1, arcs_9_5, 1}, {1, arcs_9_6, 0}};

static DFA dfa_9 = {"declaration", 7, states_9, 3, first_9};

static Arc arcs_10_0[1] = {{42, 1}};

static Arc arcs_10_1[3] = {{29, 0}, {30, 0}, {0, 1}};

static int first_10[6] = {8, 12, 13, 14, 29, 30};

static DFAState states_10[2] = {{1, arcs_10_0, 0}, {3, arcs_10_1, 1}};

static DFA dfa_10 = {"expr", 2, states_10, 6, first_10};

static Arc arcs_11_0[3] = {{29, 1}, {30, 1}, {43, 2}};

static Arc arcs_11_1[1] = {{44, 2}};

static Arc arcs_11_2[1] = {{0, 2}};

static int first_11[6] = {8, 12, 13, 14, 29, 30};

static DFAState states_11[3] = {
    {3, arcs_11_0, 0}, {1, arcs_11_1, 0}, {1, arcs_11_2, 1}};

static DFA dfa_11 = {"factor", 3, states_11, 6, first_11};

static Arc arcs_12_0[6] = {{45, 1}, {46, 1}, {47, 2},
                           {48, 2}, {49, 1}, {50, 2}};

static Arc arcs_12_1[1] = {{0, 1}};

static Arc arcs_12_2[1] = {{33, 1}};

static int first_12[6] = {51, 52, 53, 54, 55, 56};

static DFAState states_12[3] = {
    {6, arcs_12_0, 0}, {1, arcs_12_1, 1}, {1, arcs_12_2, 0}};

static DFA dfa_12 = {"flow_stmt", 3, states_12, 6, first_12};

static Arc arcs_13_0[1] = {{51, 1}};

static Arc arcs_13_1[1] = {{8, 2}};

static Arc arcs_13_2[1] = {{13, 3}};

static Arc arcs_13_3[1] = {{34, 4}};

static Arc arcs_13_4[1] = {{9, 5}};

static Arc arcs_13_5[1] = {{33, 6}};

static Arc arcs_13_6[1] = {{57, 7}};

static Arc arcs_13_7[1] = {{33, 8}};

static Arc arcs_13_8[1] = {{13, 9}};

static Arc arcs_13_9[1] = {{34, 10}};

static Arc arcs_13_10[1] = {{13, 11}};

static Arc arcs_13_11[2] = {{29, 12}, {30, 12}};

static Arc arcs_13_12[1] = {{14, 13}};

static Arc arcs_13_13[1] = {{10, 14}};

static Arc arcs_13_14[1] = {{21, 15}};

static Arc arcs_13_15[1] = {{0, 15}};

static int first_13[1] = {51};

static DFAState states_13[16] = {
    {1, arcs_13_0, 0},  {1, arcs_13_1, 0},  {1, arcs_13_2, 0},
    {1, arcs_13_3, 0},  {1, arcs_13_4, 0},  {1, arcs_13_5, 0},
    {1, arcs_13_6, 0},  {1, arcs_13_7, 0},  {1, arcs_13_8, 0},
    {1, arcs_13_9, 0},  {1, arcs_13_10, 0}, {2, arcs_13_11, 0},
    {1, arcs_13_12, 0}, {1, arcs_13_13, 0}, {1, arcs_13_14, 0},
    {1, arcs_13_15, 1}};

static DFA dfa_13 = {"for_stmt", 16, states_13, 1, first_13};

static Arc arcs_14_0[1] = {{52, 1}};

static Arc arcs_14_1[1] = {{8, 2}};

static Arc arcs_14_2[1] = {{57, 3}};

static Arc arcs_14_3[1] = {{10, 4}};

static Arc arcs_14_4[1] = {{21, 5}};

static Arc arcs_14_5[2] = {{58, 6}, {0, 5}};

static Arc arcs_14_6[1] = {{21, 7}};

static Arc arcs_14_7[1] = {{0, 7}};

static int first_14[1] = {52};

static DFAState states_14[8] = {
    {1, arcs_14_0, 0}, {1, arcs_14_1, 0}, {1, arcs_14_2, 0}, {1, arcs_14_3, 0},
    {1, arcs_14_4, 0}, {2, arcs_14_5, 1}, {1, arcs_14_6, 0}, {1, arcs_14_7, 1}};

static DFA dfa_14 = {"if_stmt", 8, states_14, 1, first_14};

static Arc arcs_15_0[3] = {{29, 1}, {30, 1}, {14, 2}};

static Arc arcs_15_1[1] = {{14, 2}};

static Arc arcs_15_2[1] = {{0, 2}};

static int first_15[3] = {29, 30, 14};

static DFAState states_15[3] = {
    {3, arcs_15_0, 0}, {1, arcs_15_1, 0}, {1, arcs_15_2, 1}};

static DFA dfa_15 = {"integer", 3, states_15, 3, first_15};

static Arc arcs_16_0[1] = {{8, 1}};

static Arc arcs_16_1[1] = {{31, 2}};

static Arc arcs_16_2[1] = {{13, 3}};

static Arc arcs_16_3[2] = {{10, 4}, {11, 1}};

static Arc arcs_16_4[1] = {{0, 4}};

static int first_16[1] = {8};

static DFAState states_16[5] = {{1, arcs_16_0, 0},
                                {1, arcs_16_1, 0},
                                {1, arcs_16_2, 0},
                                {2, arcs_16_3, 0},
                                {1, arcs_16_4, 1}};

static DFA dfa_16 = {"paralist", 5, states_16, 1, first_16};

static Arc arcs_17_0[1] = {{55, 1}};

static Arc arcs_17_1[1] = {{8, 2}};

static Arc arcs_17_2[1] = {{13, 3}};

static Arc arcs_17_3[2] = {{10, 4}, {11, 2}};

static Arc arcs_17_4[1] = {{0, 4}};

static int first_17[1] = {55};

static DFAState states_17[5] = {{1, arcs_17_0, 0},
                                {1, arcs_17_1, 0},
                                {1, arcs_17_2, 0},
                                {2, arcs_17_3, 0},
                                {1, arcs_17_4, 1}};

static DFA dfa_17 = {"read_stmt", 5, states_17, 1, first_17};

static Arc arcs_18_0[1] = {{54, 1}};

static Arc arcs_18_1[2] = {{8, 2}, {0, 1}};

static Arc arcs_18_2[1] = {{9, 3}};

static Arc arcs_18_3[1] = {{10, 4}};

static Arc arcs_18_4[1] = {{0, 4}};

static int first_18[1] = {54};

static DFAState states_18[5] = {{1, arcs_18_0, 0},
                                {2, arcs_18_1, 1},
                                {1, arcs_18_2, 0},
                                {1, arcs_18_3, 0},
                                {1, arcs_18_4, 1}};

static DFA dfa_18 = {"return_stmt", 5, states_18, 1, first_18};

static Arc arcs_19_0[4] = {{33, 1}, {19, 2}, {13, 3}, {59, 1}};

static Arc arcs_19_1[1] = {{0, 1}};

static Arc arcs_19_2[2] = {{20, 1}, {21, 2}};

static Arc arcs_19_3[2] = {{33, 1}, {60, 4}};

static Arc arcs_19_4[1] = {{33, 1}};

static int first_19[9] = {33, 13, 51, 52, 53, 54, 55, 56, 19};

static DFAState states_19[5] = {{4, arcs_19_0, 0},
                                {1, arcs_19_1, 1},
                                {2, arcs_19_2, 0},
                                {2, arcs_19_3, 0},
                                {1, arcs_19_4, 0}};

static DFA dfa_19 = {"stmt", 5, states_19, 9, first_19};

static Arc arcs_20_0[3] = {{34, 1}, {16, 2}, {17, 3}};

static Arc arcs_20_1[1] = {{9, 3}};

static Arc arcs_20_2[1] = {{9, 4}};

static Arc arcs_20_3[1] = {{0, 3}};

static Arc arcs_20_4[1] = {{18, 5}};

static Arc arcs_20_5[1] = {{34, 1}};

static int first_20[3] = {8, 16, 34};

static DFAState states_20[6] = {{3, arcs_20_0, 0}, {1, arcs_20_1, 0},
                                {1, arcs_20_2, 0}, {1, arcs_20_3, 1},
                                {1, arcs_20_4, 0}, {1, arcs_20_5, 0}};

static DFA dfa_20 = {"stmt_trailer", 6, states_20, 3, first_20};

static Arc arcs_21_0[1] = {{16, 1}};

static Arc arcs_21_1[1] = {{14, 2}};

static Arc arcs_21_2[1] = {{18, 3}};

static Arc arcs_21_3[1] = {{0, 3}};

static int first_21[1] = {16};

static DFAState states_21[4] = {
    {1, arcs_21_0, 0}, {1, arcs_21_1, 0}, {1, arcs_21_2, 0}, {1, arcs_21_3, 1}};

static DFA dfa_21 = {"subscript2", 4, states_21, 1, first_21};

static Arc arcs_22_0[1] = {{44, 1}};

static Arc arcs_22_1[3] = {{61, 0}, {62, 0}, {0, 1}};

static int first_22[6] = {8, 12, 13, 14, 29, 30};

static DFAState states_22[2] = {{1, arcs_22_0, 0}, {3, arcs_22_1, 1}};

static DFA dfa_22 = {"term", 2, states_22, 6, first_22};

static Arc arcs_23_0[3] = {{4, 1}, {6, 1}, {7, 1}};

static Arc arcs_23_1[1] = {{0, 1}};

static int first_23[3] = {4, 6, 7};

static DFAState states_23[2] = {{3, arcs_23_0, 0}, {1, arcs_23_1, 1}};

static DFA dfa_23 = {"type_name", 2, states_23, 3, first_23};

static Arc arcs_24_0[1] = {{31, 1}};

static Arc arcs_24_1[1] = {{39, 2}};

static Arc arcs_24_2[2] = {{11, 1}, {33, 3}};

static Arc arcs_24_3[1] = {{0, 3}};

static int first_24[3] = {4, 6, 7};

static DFAState states_24[4] = {
    {1, arcs_24_0, 0}, {1, arcs_24_1, 0}, {2, arcs_24_2, 0}, {1, arcs_24_3, 1}};

static DFA dfa_24 = {"var_decl", 4, states_24, 3, first_24};

static Arc arcs_25_0[1] = {{13, 1}};

static Arc arcs_25_1[2] = {{38, 2}, {0, 1}};

static Arc arcs_25_2[1] = {{0, 2}};

static int first_25[1] = {13};

static DFAState states_25[3] = {
    {1, arcs_25_0, 0}, {2, arcs_25_1, 1}, {1, arcs_25_2, 1}};

static DFA dfa_25 = {"var_item", 3, states_25, 1, first_25};

static Arc arcs_26_0[1] = {{56, 1}};

static Arc arcs_26_1[1] = {{8, 2}};

static Arc arcs_26_2[1] = {{57, 3}};

static Arc arcs_26_3[1] = {{10, 4}};

static Arc arcs_26_4[1] = {{21, 5}};

static Arc arcs_26_5[1] = {{0, 5}};

static int first_26[1] = {56};

static DFAState states_26[6] = {{1, arcs_26_0, 0}, {1, arcs_26_1, 0},
                                {1, arcs_26_2, 0}, {1, arcs_26_3, 0},
                                {1, arcs_26_4, 0}, {1, arcs_26_5, 1}};

static DFA dfa_26 = {"while_stmt", 6, states_26, 1, first_26};

static Arc arcs_27_0[1] = {{53, 1}};

static Arc arcs_27_1[1] = {{8, 2}};

static Arc arcs_27_2[2] = {{63, 3}, {9, 4}};

static Arc arcs_27_3[2] = {{10, 5}, {11, 6}};

static Arc arcs_27_4[1] = {{10, 5}};

static Arc arcs_27_5[1] = {{0, 5}};

static Arc arcs_27_6[1] = {{9, 4}};

static int first_27[1] = {53};

static DFAState states_27[7] = {
    {1, arcs_27_0, 0}, {1, arcs_27_1, 0}, {2, arcs_27_2, 0}, {2, arcs_27_3, 0},
    {1, arcs_27_4, 0}, {1, arcs_27_5, 1}, {1, arcs_27_6, 0}};

static DFA dfa_27 = {"write_stmt", 7, states_27, 1, first_27};

static DFA *dfas[28] = {&dfa_0,  &dfa_1,  &dfa_2,  &dfa_3,  &dfa_4,  &dfa_5,
                        &dfa_6,  &dfa_7,  &dfa_8,  &dfa_9,  &dfa_10, &dfa_11,
                        &dfa_12, &dfa_13, &dfa_14, &dfa_15, &dfa_16, &dfa_17,
                        &dfa_18, &dfa_19, &dfa_20, &dfa_21, &dfa_22, &dfa_23,
                        &dfa_24, &dfa_25, &dfa_26, &dfa_27};

Grammar CompilerGrammar = {256, 28, 64, labels, dfas};

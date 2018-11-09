#include "grammar.h"

const char *TokenNames[7] = {
    "NAME",
    "OP",
    "ERRORTOKEN",
    "ENDMARKER",
    "CHAR",
    "NUMBER",
    "STRING",
};

const char *SymbolNames[28] = {
    "program",
    "arglist",
    "compound_stmt",
    "condition",
    "const_decl",
    "const_item",
    "decl_trailer",
    "declaration",
    "expr",
    "factor",
    "factor_trailer",
    "for_stmt",
    "if_stmt",
    "integer",
    "konstant",
    "paralist",
    "read_stmt",
    "return_stmt",
    "stmt",
    "stmt_trailer",
    "subscript",
    "subscript2",
    "term",
    "type_name",
    "var_decl",
    "var_item",
    "while_stmt",
    "write_stmt",
};

static Label labels[64] = {
    {0, "EMPTY"},
    {3, 0},
    {260, 0},
    {263, 0},
    {0, "char"},
    {0, "const"},
    {0, "int"},
    {0, "void"},
    {264, 0},
    {0, ","},
    {0, "("},
    {0, "+"},
    {0, "-"},
    {4, 0},
    {0, 0},
    {5, 0},
    {274, 0},
    {280, 0},
    {0, ";"},
    {0, "for"},
    {0, "if"},
    {0, "printf"},
    {0, "return"},
    {0, "scanf"},
    {0, "while"},
    {0, "{"},
    {0, "!="},
    {0, "<"},
    {0, "<="},
    {0, "=="},
    {0, ">"},
    {0, ">="},
    {279, 0},
    {261, 0},
    {0, "="},
    {270, 0},
    {277, 0},
    {271, 0},
    {281, 0},
    {258, 0},
    {0, ")"},
    {0, "}"},
    {0, "["},
    {0, "main"},
    {262, 0},
    {278, 0},
    {266, 0},
    {276, 0},
    {257, 0},
    {259, 0},
    {0, "else"},
    {269, 0},
    {267, 0},
    {268, 0},
    {272, 0},
    {273, 0},
    {282, 0},
    {283, 0},
    {275, 0},
    {0, "]"},
    {265, 0},
    {0, "*"},
    {0, "/"},
    {6, 0},
};

static Arc arcs_0_0[3] = { { 1, 1 }, { 2, 0 }, { 3, 2 } };

static Arc arcs_0_1[1] = { { 0, 1 } };

static Arc arcs_0_2[2] = { { 1, 1 }, { 3, 2 } };

static int first_0[5] = { 1, 4, 5, 6, 7 };

static DFAState states_0[3] = { { 3, arcs_0_0, 0 }, { 1, arcs_0_1, 1 }, { 2,
                                arcs_0_2, 0 } };

static DFA dfa_0 = { "program", 3, states_0, first_0, 5 };

static Arc arcs_1_0[1] = { { 8, 1 } };

static Arc arcs_1_1[2] = { { 9, 0 }, { 0, 1 } };

static int first_1[6] = { 10, 11, 12, 13, 14, 15 };

static DFAState states_1[2] = { { 1, arcs_1_0, 0 }, { 2, arcs_1_1, 1 } };

static DFA dfa_1 = { "arglist", 2, states_1, first_1, 6 };

static Arc arcs_2_0[4] = { { 2, 0 }, { 16, 1 }, { 17, 2 }, { 0, 0 } };

static Arc arcs_2_1[2] = { { 16, 1 }, { 0, 1 } };

static Arc arcs_2_2[3] = { { 16, 1 }, { 17, 2 }, { 0, 2 } };

static int first_2[13] = { 4, 5, 6, 7, 14, 18, 19, 20, 21, 22, 23, 24, 25 };

static DFAState states_2[3] = { { 4, arcs_2_0, 1 }, { 2, arcs_2_1, 1 }, { 3,
                                arcs_2_2, 1 } };

static DFA dfa_2 = { "compound_stmt", 3, states_2, first_2, 13 };

static Arc arcs_3_0[1] = { { 8, 1 } };

static Arc arcs_3_1[7] = { { 26, 2 }, { 27, 2 }, { 28, 2 }, { 29, 2 }, { 30, 2
                           }, { 31, 2 }, { 0, 1 } };

static Arc arcs_3_2[1] = { { 8, 3 } };

static Arc arcs_3_3[1] = { { 0, 3 } };

static int first_3[6] = { 10, 11, 12, 13, 14, 15 };

static DFAState states_3[4] = { { 1, arcs_3_0, 0 }, { 7, arcs_3_1, 1 }, { 1,
                                arcs_3_2, 0 }, { 1, arcs_3_3, 1 } };

static DFA dfa_3 = { "condition", 4, states_3, first_3, 6 };

static Arc arcs_4_0[1] = { { 5, 1 } };

static Arc arcs_4_1[1] = { { 32, 2 } };

static Arc arcs_4_2[1] = { { 33, 3 } };

static Arc arcs_4_3[2] = { { 9, 2 }, { 18, 4 } };

static Arc arcs_4_4[1] = { { 0, 4 } };

static int first_4[1] = { 5 };

static DFAState states_4[5] = { { 1, arcs_4_0, 0 }, { 1, arcs_4_1, 0 }, { 1,
                                arcs_4_2, 0 }, { 2, arcs_4_3, 0 }, { 1,
                                arcs_4_4, 1 } };

static DFA dfa_4 = { "const_decl", 5, states_4, first_4, 1 };

static Arc arcs_5_0[1] = { { 14, 1 } };

static Arc arcs_5_1[1] = { { 34, 2 } };

static Arc arcs_5_2[1] = { { 35, 3 } };

static Arc arcs_5_3[1] = { { 0, 3 } };

static int first_5[1] = { 14 };

static DFAState states_5[4] = { { 1, arcs_5_0, 0 }, { 1, arcs_5_1, 0 }, { 1,
                                arcs_5_2, 0 }, { 1, arcs_5_3, 1 } };

static DFA dfa_5 = { "const_item", 4, states_5, first_5, 1 };

static Arc arcs_6_0[5] = { { 10, 1 }, { 9, 2 }, { 18, 3 }, { 25, 4 }, { 36, 5 }
                           };

static Arc arcs_6_1[1] = { { 37, 6 } };

static Arc arcs_6_2[1] = { { 38, 5 } };

static Arc arcs_6_3[1] = { { 0, 3 } };

static Arc arcs_6_4[1] = { { 39, 7 } };

static Arc arcs_6_5[2] = { { 9, 2 }, { 18, 3 } };

static Arc arcs_6_6[1] = { { 40, 8 } };

static Arc arcs_6_7[1] = { { 41, 3 } };

static Arc arcs_6_8[1] = { { 25, 4 } };

static int first_6[5] = { 9, 10, 42, 18, 25 };

static DFAState states_6[9] = { { 5, arcs_6_0, 0 }, { 1, arcs_6_1, 0 }, { 1,
                                arcs_6_2, 0 }, { 1, arcs_6_3, 1 }, { 1,
                                arcs_6_4, 0 }, { 2, arcs_6_5, 0 }, { 1,
                                arcs_6_6, 0 }, { 1, arcs_6_7, 0 }, { 1,
                                arcs_6_8, 0 } };

static DFA dfa_6 = { "decl_trailer", 9, states_6, first_6, 5 };

static Arc arcs_7_0[1] = { { 32, 1 } };

static Arc arcs_7_1[2] = { { 43, 2 }, { 14, 3 } };

static Arc arcs_7_2[1] = { { 10, 4 } };

static Arc arcs_7_3[1] = { { 44, 5 } };

static Arc arcs_7_4[1] = { { 40, 6 } };

static Arc arcs_7_5[1] = { { 0, 5 } };

static Arc arcs_7_6[1] = { { 25, 7 } };

static Arc arcs_7_7[1] = { { 39, 8 } };

static Arc arcs_7_8[1] = { { 41, 5 } };

static int first_7[3] = { 4, 6, 7 };

static DFAState states_7[9] = { { 1, arcs_7_0, 0 }, { 2, arcs_7_1, 0 }, { 1,
                                arcs_7_2, 0 }, { 1, arcs_7_3, 0 }, { 1,
                                arcs_7_4, 0 }, { 1, arcs_7_5, 1 }, { 1,
                                arcs_7_6, 0 }, { 1, arcs_7_7, 0 }, { 1,
                                arcs_7_8, 0 } };

static DFA dfa_7 = { "declaration", 9, states_7, first_7, 3 };

static Arc arcs_8_0[3] = { { 11, 1 }, { 12, 1 }, { 45, 2 } };

static Arc arcs_8_1[1] = { { 45, 2 } };

static Arc arcs_8_2[3] = { { 11, 1 }, { 12, 1 }, { 0, 2 } };

static int first_8[6] = { 10, 11, 12, 13, 14, 15 };

static DFAState states_8[3] = { { 3, arcs_8_0, 0 }, { 1, arcs_8_1, 0 }, { 3,
                                arcs_8_2, 1 } };

static DFA dfa_8 = { "expr", 3, states_8, first_8, 6 };

static Arc arcs_9_0[4] = { { 10, 1 }, { 13, 2 }, { 14, 3 }, { 15, 2 } };

static Arc arcs_9_1[1] = { { 8, 4 } };

static Arc arcs_9_2[1] = { { 0, 2 } };

static Arc arcs_9_3[2] = { { 46, 2 }, { 0, 3 } };

static Arc arcs_9_4[1] = { { 40, 2 } };

static int first_9[4] = { 10, 13, 14, 15 };

static DFAState states_9[5] = { { 4, arcs_9_0, 0 }, { 1, arcs_9_1, 0 }, { 1,
                                arcs_9_2, 1 }, { 2, arcs_9_3, 1 }, { 1,
                                arcs_9_4, 0 } };

static DFA dfa_9 = { "factor", 5, states_9, first_9, 4 };

static Arc arcs_10_0[2] = { { 10, 1 }, { 47, 2 } };

static Arc arcs_10_1[2] = { { 40, 2 }, { 48, 3 } };

static Arc arcs_10_2[1] = { { 0, 2 } };

static Arc arcs_10_3[1] = { { 40, 2 } };

static int first_10[2] = { 10, 42 };

static DFAState states_10[4] = { { 2, arcs_10_0, 0 }, { 2, arcs_10_1, 0 }, { 1,
                                 arcs_10_2, 1 }, { 1, arcs_10_3, 0 } };

static DFA dfa_10 = { "factor_trailer", 4, states_10, first_10, 2 };

static Arc arcs_11_0[1] = { { 19, 1 } };

static Arc arcs_11_1[1] = { { 10, 2 } };

static Arc arcs_11_2[1] = { { 14, 3 } };

static Arc arcs_11_3[1] = { { 34, 4 } };

static Arc arcs_11_4[1] = { { 8, 5 } };

static Arc arcs_11_5[1] = { { 18, 6 } };

static Arc arcs_11_6[1] = { { 49, 7 } };

static Arc arcs_11_7[1] = { { 18, 8 } };

static Arc arcs_11_8[1] = { { 14, 9 } };

static Arc arcs_11_9[1] = { { 34, 10 } };

static Arc arcs_11_10[1] = { { 14, 11 } };

static Arc arcs_11_11[2] = { { 11, 12 }, { 12, 12 } };

static Arc arcs_11_12[1] = { { 15, 13 } };

static Arc arcs_11_13[1] = { { 40, 14 } };

static Arc arcs_11_14[1] = { { 16, 15 } };

static Arc arcs_11_15[1] = { { 0, 15 } };

static int first_11[1] = { 19 };

static DFAState states_11[16] = { { 1, arcs_11_0, 0 }, { 1, arcs_11_1, 0 }, {
                                  1, arcs_11_2, 0 }, { 1, arcs_11_3, 0 }, { 1,
                                  arcs_11_4, 0 }, { 1, arcs_11_5, 0 }, { 1,
                                  arcs_11_6, 0 }, { 1, arcs_11_7, 0 }, { 1,
                                  arcs_11_8, 0 }, { 1, arcs_11_9, 0 }, { 1,
                                  arcs_11_10, 0 }, { 2, arcs_11_11, 0 }, { 1,
                                  arcs_11_12, 0 }, { 1, arcs_11_13, 0 }, { 1,
                                  arcs_11_14, 0 }, { 1, arcs_11_15, 1 } };

static DFA dfa_11 = { "for_stmt", 16, states_11, first_11, 1 };

static Arc arcs_12_0[1] = { { 20, 1 } };

static Arc arcs_12_1[1] = { { 10, 2 } };

static Arc arcs_12_2[1] = { { 49, 3 } };

static Arc arcs_12_3[1] = { { 40, 4 } };

static Arc arcs_12_4[1] = { { 16, 5 } };

static Arc arcs_12_5[2] = { { 50, 6 }, { 0, 5 } };

static Arc arcs_12_6[1] = { { 16, 7 } };

static Arc arcs_12_7[1] = { { 0, 7 } };

static int first_12[1] = { 20 };

static DFAState states_12[8] = { { 1, arcs_12_0, 0 }, { 1, arcs_12_1, 0 }, { 1,
                                 arcs_12_2, 0 }, { 1, arcs_12_3, 0 }, { 1,
                                 arcs_12_4, 0 }, { 2, arcs_12_5, 1 }, { 1,
                                 arcs_12_6, 0 }, { 1, arcs_12_7, 1 } };

static DFA dfa_12 = { "if_stmt", 8, states_12, first_12, 1 };

static Arc arcs_13_0[3] = { { 11, 1 }, { 12, 1 }, { 15, 2 } };

static Arc arcs_13_1[1] = { { 15, 2 } };

static Arc arcs_13_2[1] = { { 0, 2 } };

static int first_13[3] = { 11, 12, 15 };

static DFAState states_13[3] = { { 3, arcs_13_0, 0 }, { 1, arcs_13_1, 0 }, { 1,
                                 arcs_13_2, 1 } };

static DFA dfa_13 = { "integer", 3, states_13, first_13, 3 };

static Arc arcs_14_0[2] = { { 13, 1 }, { 51, 1 } };

static Arc arcs_14_1[1] = { { 0, 1 } };

static int first_14[4] = { 11, 12, 13, 15 };

static DFAState states_14[2] = { { 2, arcs_14_0, 0 }, { 1, arcs_14_1, 1 } };

static DFA dfa_14 = { "konstant", 2, states_14, first_14, 4 };

static Arc arcs_15_0[1] = { { 32, 1 } };

static Arc arcs_15_1[1] = { { 14, 2 } };

static Arc arcs_15_2[2] = { { 9, 0 }, { 0, 2 } };

static int first_15[3] = { 4, 6, 7 };

static DFAState states_15[3] = { { 1, arcs_15_0, 0 }, { 1, arcs_15_1, 0 }, { 2,
                                 arcs_15_2, 1 } };

static DFA dfa_15 = { "paralist", 3, states_15, first_15, 3 };

static Arc arcs_16_0[1] = { { 23, 1 } };

static Arc arcs_16_1[1] = { { 10, 2 } };

static Arc arcs_16_2[1] = { { 14, 3 } };

static Arc arcs_16_3[2] = { { 40, 4 }, { 9, 2 } };

static Arc arcs_16_4[1] = { { 0, 4 } };

static int first_16[1] = { 23 };

static DFAState states_16[5] = { { 1, arcs_16_0, 0 }, { 1, arcs_16_1, 0 }, { 1,
                                 arcs_16_2, 0 }, { 2, arcs_16_3, 0 }, { 1,
                                 arcs_16_4, 1 } };

static DFA dfa_16 = { "read_stmt", 5, states_16, first_16, 1 };

static Arc arcs_17_0[1] = { { 22, 1 } };

static Arc arcs_17_1[2] = { { 10, 2 }, { 0, 1 } };

static Arc arcs_17_2[1] = { { 8, 3 } };

static Arc arcs_17_3[1] = { { 40, 4 } };

static Arc arcs_17_4[1] = { { 0, 4 } };

static int first_17[1] = { 22 };

static DFAState states_17[5] = { { 1, arcs_17_0, 0 }, { 2, arcs_17_1, 1 }, { 1,
                                 arcs_17_2, 0 }, { 1, arcs_17_3, 0 }, { 1,
                                 arcs_17_4, 1 } };

static DFA dfa_17 = { "return_stmt", 5, states_17, first_17, 1 };

static Arc arcs_18_0[9] = { { 18, 1 }, { 25, 2 }, { 14, 3 }, { 52, 1 }, { 53, 1
                            }, { 54, 4 }, { 55, 4 }, { 56, 1 }, { 57, 4 } };

static Arc arcs_18_1[1] = { { 0, 1 } };

static Arc arcs_18_2[2] = { { 41, 1 }, { 16, 2 } };

static Arc arcs_18_3[1] = { { 58, 4 } };

static Arc arcs_18_4[1] = { { 18, 1 } };

static int first_18[9] = { 14, 18, 19, 20, 21, 22, 23, 24, 25 };

static DFAState states_18[5] = { { 9, arcs_18_0, 0 }, { 1, arcs_18_1, 1 }, { 2,
                                 arcs_18_2, 0 }, { 1, arcs_18_3, 0 }, { 1,
                                 arcs_18_4, 0 } };

static DFA dfa_18 = { "stmt", 5, states_18, first_18, 9 };

static Arc arcs_19_0[4] = { { 10, 1 }, { 34, 2 }, { 47, 3 }, { 0, 0 } };

static Arc arcs_19_1[1] = { { 48, 4 } };

static Arc arcs_19_2[1] = { { 8, 5 } };

static Arc arcs_19_3[1] = { { 34, 2 } };

static Arc arcs_19_4[1] = { { 40, 5 } };

static Arc arcs_19_5[1] = { { 0, 5 } };

static int first_19[3] = { 10, 34, 42 };

static DFAState states_19[6] = { { 4, arcs_19_0, 1 }, { 1, arcs_19_1, 0 }, { 1,
                                 arcs_19_2, 0 }, { 1, arcs_19_3, 0 }, { 1,
                                 arcs_19_4, 0 }, { 1, arcs_19_5, 1 } };

static DFA dfa_19 = { "stmt_trailer", 6, states_19, first_19, 3 };

static Arc arcs_20_0[1] = { { 42, 1 } };

static Arc arcs_20_1[1] = { { 8, 2 } };

static Arc arcs_20_2[1] = { { 59, 3 } };

static Arc arcs_20_3[1] = { { 0, 3 } };

static int first_20[1] = { 42 };

static DFAState states_20[4] = { { 1, arcs_20_0, 0 }, { 1, arcs_20_1, 0 }, { 1,
                                 arcs_20_2, 0 }, { 1, arcs_20_3, 1 } };

static DFA dfa_20 = { "subscript", 4, states_20, first_20, 1 };

static Arc arcs_21_0[1] = { { 42, 1 } };

static Arc arcs_21_1[1] = { { 15, 2 } };

static Arc arcs_21_2[1] = { { 59, 3 } };

static Arc arcs_21_3[1] = { { 0, 3 } };

static int first_21[1] = { 42 };

static DFAState states_21[4] = { { 1, arcs_21_0, 0 }, { 1, arcs_21_1, 0 }, { 1,
                                 arcs_21_2, 0 }, { 1, arcs_21_3, 1 } };

static DFA dfa_21 = { "subscript2", 4, states_21, first_21, 1 };

static Arc arcs_22_0[1] = { { 60, 1 } };

static Arc arcs_22_1[3] = { { 61, 0 }, { 62, 0 }, { 0, 1 } };

static int first_22[4] = { 10, 13, 14, 15 };

static DFAState states_22[2] = { { 1, arcs_22_0, 0 }, { 3, arcs_22_1, 1 } };

static DFA dfa_22 = { "term", 2, states_22, first_22, 4 };

static Arc arcs_23_0[3] = { { 4, 1 }, { 6, 1 }, { 7, 1 } };

static Arc arcs_23_1[1] = { { 0, 1 } };

static int first_23[3] = { 4, 6, 7 };

static DFAState states_23[2] = { { 3, arcs_23_0, 0 }, { 1, arcs_23_1, 1 } };

static DFA dfa_23 = { "type_name", 2, states_23, first_23, 3 };

static Arc arcs_24_0[1] = { { 32, 1 } };

static Arc arcs_24_1[1] = { { 38, 2 } };

static Arc arcs_24_2[2] = { { 9, 1 }, { 18, 3 } };

static Arc arcs_24_3[1] = { { 0, 3 } };

static int first_24[3] = { 4, 6, 7 };

static DFAState states_24[4] = { { 1, arcs_24_0, 0 }, { 1, arcs_24_1, 0 }, { 2,
                                 arcs_24_2, 0 }, { 1, arcs_24_3, 1 } };

static DFA dfa_24 = { "var_decl", 4, states_24, first_24, 3 };

static Arc arcs_25_0[1] = { { 14, 1 } };

static Arc arcs_25_1[2] = { { 36, 2 }, { 0, 1 } };

static Arc arcs_25_2[1] = { { 0, 2 } };

static int first_25[1] = { 14 };

static DFAState states_25[3] = { { 1, arcs_25_0, 0 }, { 2, arcs_25_1, 1 }, { 1,
                                 arcs_25_2, 1 } };

static DFA dfa_25 = { "var_item", 3, states_25, first_25, 1 };

static Arc arcs_26_0[1] = { { 24, 1 } };

static Arc arcs_26_1[1] = { { 10, 2 } };

static Arc arcs_26_2[1] = { { 49, 3 } };

static Arc arcs_26_3[1] = { { 40, 4 } };

static Arc arcs_26_4[1] = { { 16, 5 } };

static Arc arcs_26_5[1] = { { 0, 5 } };

static int first_26[1] = { 24 };

static DFAState states_26[6] = { { 1, arcs_26_0, 0 }, { 1, arcs_26_1, 0 }, { 1,
                                 arcs_26_2, 0 }, { 1, arcs_26_3, 0 }, { 1,
                                 arcs_26_4, 0 }, { 1, arcs_26_5, 1 } };

static DFA dfa_26 = { "while_stmt", 6, states_26, first_26, 1 };

static Arc arcs_27_0[1] = { { 21, 1 } };

static Arc arcs_27_1[1] = { { 10, 2 } };

static Arc arcs_27_2[2] = { { 63, 3 }, { 8, 4 } };

static Arc arcs_27_3[2] = { { 40, 5 }, { 9, 6 } };

static Arc arcs_27_4[1] = { { 40, 5 } };

static Arc arcs_27_5[1] = { { 0, 5 } };

static Arc arcs_27_6[1] = { { 8, 4 } };

static int first_27[1] = { 21 };

static DFAState states_27[7] = { { 1, arcs_27_0, 0 }, { 1, arcs_27_1, 0 }, { 2,
                                 arcs_27_2, 0 }, { 2, arcs_27_3, 0 }, { 1,
                                 arcs_27_4, 0 }, { 1, arcs_27_5, 1 }, { 1,
                                 arcs_27_6, 0 } };

static DFA dfa_27 = { "write_stmt", 7, states_27, first_27, 1 };

static DFA *dfas[28] = { &dfa_0, &dfa_1, &dfa_2, &dfa_3, &dfa_4, &dfa_5,
                         &dfa_6, &dfa_7, &dfa_8, &dfa_9, &dfa_10, &dfa_11,
                         &dfa_12, &dfa_13, &dfa_14, &dfa_15, &dfa_16, &dfa_17,
                         &dfa_18, &dfa_19, &dfa_20, &dfa_21, &dfa_22, &dfa_23,
                         &dfa_24, &dfa_25, &dfa_26, &dfa_27 };

Grammar CompilerGrammar = { 256, 28, 64, labels, dfas };

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

const char *SymbolNames[30] = {
    "program",
    "arglist",
    "case_stmt",
    "compound_stmt",
    "condition",
    "const_decl",
    "const_item",
    "decl_trailer",
    "declaration",
    "default_stmt",
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
    "switch_stmt",
    "term",
    "type_name",
    "var_decl",
    "var_item",
    "write_stmt",
};

static Label labels[68] = {
    {0, "EMPTY"},
    {3, 0},
    {261, 0},
    {264, 0},
    {0, "char"},
    {0, "const"},
    {0, "int"},
    {0, "void"},
    {266, 0},
    {0, ","},
    {0, "("},
    {0, "+"},
    {0, "-"},
    {4, 0},
    {0, 0},
    {5, 0},
    {0, "case"},
    {272, 0},
    {0, ":"},
    {276, 0},
    {283, 0},
    {0, ";"},
    {0, "for"},
    {0, "if"},
    {0, "printf"},
    {0, "return"},
    {0, "scanf"},
    {0, "switch"},
    {0, "{"},
    {0, "!="},
    {0, "<"},
    {0, "<="},
    {0, "=="},
    {0, ">"},
    {0, ">="},
    {282, 0},
    {262, 0},
    {0, "="},
    {279, 0},
    {0, ")"},
    {273, 0},
    {284, 0},
    {259, 0},
    {0, "}"},
    {0, "["},
    {263, 0},
    {0, "default"},
    {281, 0},
    {268, 0},
    {278, 0},
    {257, 0},
    {260, 0},
    {0, "else"},
    {271, 0},
    {269, 0},
    {270, 0},
    {274, 0},
    {275, 0},
    {280, 0},
    {285, 0},
    {277, 0},
    {0, "]"},
    {258, 0},
    {265, 0},
    {267, 0},
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

static Arc arcs_2_0[1] = { { 16, 1 } };

static Arc arcs_2_1[1] = { { 17, 2 } };

static Arc arcs_2_2[1] = { { 18, 3 } };

static Arc arcs_2_3[1] = { { 19, 4 } };

static Arc arcs_2_4[1] = { { 0, 4 } };

static int first_2[1] = { 16 };

static DFAState states_2[5] = { { 1, arcs_2_0, 0 }, { 1, arcs_2_1, 0 }, { 1,
                                arcs_2_2, 0 }, { 1, arcs_2_3, 0 }, { 1,
                                arcs_2_4, 1 } };

static DFA dfa_2 = { "case_stmt", 5, states_2, first_2, 1 };

static Arc arcs_3_0[4] = { { 2, 0 }, { 19, 1 }, { 20, 2 }, { 0, 0 } };

static Arc arcs_3_1[2] = { { 19, 1 }, { 0, 1 } };

static Arc arcs_3_2[3] = { { 19, 1 }, { 20, 2 }, { 0, 2 } };

static int first_3[13] = { 4, 5, 6, 7, 14, 21, 22, 23, 24, 25, 26, 27, 28 };

static DFAState states_3[3] = { { 4, arcs_3_0, 1 }, { 2, arcs_3_1, 1 }, { 3,
                                arcs_3_2, 1 } };

static DFA dfa_3 = { "compound_stmt", 3, states_3, first_3, 13 };

static Arc arcs_4_0[1] = { { 8, 1 } };

static Arc arcs_4_1[7] = { { 29, 2 }, { 30, 2 }, { 31, 2 }, { 32, 2 }, { 33, 2
                           }, { 34, 2 }, { 0, 1 } };

static Arc arcs_4_2[1] = { { 8, 3 } };

static Arc arcs_4_3[1] = { { 0, 3 } };

static int first_4[6] = { 10, 11, 12, 13, 14, 15 };

static DFAState states_4[4] = { { 1, arcs_4_0, 0 }, { 7, arcs_4_1, 1 }, { 1,
                                arcs_4_2, 0 }, { 1, arcs_4_3, 1 } };

static DFA dfa_4 = { "condition", 4, states_4, first_4, 6 };

static Arc arcs_5_0[1] = { { 5, 1 } };

static Arc arcs_5_1[1] = { { 35, 2 } };

static Arc arcs_5_2[1] = { { 36, 3 } };

static Arc arcs_5_3[2] = { { 9, 2 }, { 21, 4 } };

static Arc arcs_5_4[1] = { { 0, 4 } };

static int first_5[1] = { 5 };

static DFAState states_5[5] = { { 1, arcs_5_0, 0 }, { 1, arcs_5_1, 0 }, { 1,
                                arcs_5_2, 0 }, { 2, arcs_5_3, 0 }, { 1,
                                arcs_5_4, 1 } };

static DFA dfa_5 = { "const_decl", 5, states_5, first_5, 1 };

static Arc arcs_6_0[1] = { { 14, 1 } };

static Arc arcs_6_1[1] = { { 37, 2 } };

static Arc arcs_6_2[1] = { { 17, 3 } };

static Arc arcs_6_3[1] = { { 0, 3 } };

static int first_6[1] = { 14 };

static DFAState states_6[4] = { { 1, arcs_6_0, 0 }, { 1, arcs_6_1, 0 }, { 1,
                                arcs_6_2, 0 }, { 1, arcs_6_3, 1 } };

static DFA dfa_6 = { "const_item", 4, states_6, first_6, 1 };

static Arc arcs_7_0[4] = { { 10, 1 }, { 9, 2 }, { 21, 3 }, { 38, 4 } };

static Arc arcs_7_1[2] = { { 39, 5 }, { 40, 6 } };

static Arc arcs_7_2[1] = { { 41, 4 } };

static Arc arcs_7_3[1] = { { 0, 3 } };

static Arc arcs_7_4[2] = { { 9, 2 }, { 21, 3 } };

static Arc arcs_7_5[1] = { { 28, 7 } };

static Arc arcs_7_6[1] = { { 39, 5 } };

static Arc arcs_7_7[1] = { { 42, 8 } };

static Arc arcs_7_8[1] = { { 43, 3 } };

static int first_7[4] = { 9, 10, 44, 21 };

static DFAState states_7[9] = { { 4, arcs_7_0, 0 }, { 2, arcs_7_1, 0 }, { 1,
                                arcs_7_2, 0 }, { 1, arcs_7_3, 1 }, { 2,
                                arcs_7_4, 0 }, { 1, arcs_7_5, 0 }, { 1,
                                arcs_7_6, 0 }, { 1, arcs_7_7, 0 }, { 1,
                                arcs_7_8, 0 } };

static DFA dfa_7 = { "decl_trailer", 9, states_7, first_7, 4 };

static Arc arcs_8_0[1] = { { 35, 1 } };

static Arc arcs_8_1[1] = { { 14, 2 } };

static Arc arcs_8_2[1] = { { 45, 3 } };

static Arc arcs_8_3[1] = { { 0, 3 } };

static int first_8[3] = { 4, 6, 7 };

static DFAState states_8[4] = { { 1, arcs_8_0, 0 }, { 1, arcs_8_1, 0 }, { 1,
                                arcs_8_2, 0 }, { 1, arcs_8_3, 1 } };

static DFA dfa_8 = { "declaration", 4, states_8, first_8, 3 };

static Arc arcs_9_0[1] = { { 46, 1 } };

static Arc arcs_9_1[1] = { { 18, 2 } };

static Arc arcs_9_2[1] = { { 19, 3 } };

static Arc arcs_9_3[1] = { { 0, 3 } };

static int first_9[1] = { 46 };

static DFAState states_9[4] = { { 1, arcs_9_0, 0 }, { 1, arcs_9_1, 0 }, { 1,
                                arcs_9_2, 0 }, { 1, arcs_9_3, 1 } };

static DFA dfa_9 = { "default_stmt", 4, states_9, first_9, 1 };

static Arc arcs_10_0[3] = { { 11, 1 }, { 12, 1 }, { 47, 2 } };

static Arc arcs_10_1[1] = { { 47, 2 } };

static Arc arcs_10_2[3] = { { 11, 1 }, { 12, 1 }, { 0, 2 } };

static int first_10[6] = { 10, 11, 12, 13, 14, 15 };

static DFAState states_10[3] = { { 3, arcs_10_0, 0 }, { 1, arcs_10_1, 0 }, { 3,
                                 arcs_10_2, 1 } };

static DFA dfa_10 = { "expr", 3, states_10, first_10, 6 };

static Arc arcs_11_0[4] = { { 10, 1 }, { 13, 2 }, { 14, 3 }, { 15, 2 } };

static Arc arcs_11_1[1] = { { 8, 4 } };

static Arc arcs_11_2[1] = { { 0, 2 } };

static Arc arcs_11_3[2] = { { 48, 2 }, { 0, 3 } };

static Arc arcs_11_4[1] = { { 39, 2 } };

static int first_11[4] = { 10, 13, 14, 15 };

static DFAState states_11[5] = { { 4, arcs_11_0, 0 }, { 1, arcs_11_1, 0 }, { 1,
                                 arcs_11_2, 1 }, { 2, arcs_11_3, 1 }, { 1,
                                 arcs_11_4, 0 } };

static DFA dfa_11 = { "factor", 5, states_11, first_11, 4 };

static Arc arcs_12_0[2] = { { 10, 1 }, { 49, 2 } };

static Arc arcs_12_1[2] = { { 39, 2 }, { 50, 3 } };

static Arc arcs_12_2[1] = { { 0, 2 } };

static Arc arcs_12_3[1] = { { 39, 2 } };

static int first_12[2] = { 10, 44 };

static DFAState states_12[4] = { { 2, arcs_12_0, 0 }, { 2, arcs_12_1, 0 }, { 1,
                                 arcs_12_2, 1 }, { 1, arcs_12_3, 0 } };

static DFA dfa_12 = { "factor_trailer", 4, states_12, first_12, 2 };

static Arc arcs_13_0[1] = { { 22, 1 } };

static Arc arcs_13_1[1] = { { 10, 2 } };

static Arc arcs_13_2[1] = { { 14, 3 } };

static Arc arcs_13_3[1] = { { 37, 4 } };

static Arc arcs_13_4[1] = { { 8, 5 } };

static Arc arcs_13_5[1] = { { 21, 6 } };

static Arc arcs_13_6[1] = { { 51, 7 } };

static Arc arcs_13_7[1] = { { 21, 8 } };

static Arc arcs_13_8[1] = { { 14, 9 } };

static Arc arcs_13_9[1] = { { 37, 10 } };

static Arc arcs_13_10[1] = { { 14, 11 } };

static Arc arcs_13_11[2] = { { 11, 12 }, { 12, 12 } };

static Arc arcs_13_12[1] = { { 15, 13 } };

static Arc arcs_13_13[1] = { { 39, 14 } };

static Arc arcs_13_14[1] = { { 19, 15 } };

static Arc arcs_13_15[1] = { { 0, 15 } };

static int first_13[1] = { 22 };

static DFAState states_13[16] = { { 1, arcs_13_0, 0 }, { 1, arcs_13_1, 0 }, {
                                  1, arcs_13_2, 0 }, { 1, arcs_13_3, 0 }, { 1,
                                  arcs_13_4, 0 }, { 1, arcs_13_5, 0 }, { 1,
                                  arcs_13_6, 0 }, { 1, arcs_13_7, 0 }, { 1,
                                  arcs_13_8, 0 }, { 1, arcs_13_9, 0 }, { 1,
                                  arcs_13_10, 0 }, { 2, arcs_13_11, 0 }, { 1,
                                  arcs_13_12, 0 }, { 1, arcs_13_13, 0 }, { 1,
                                  arcs_13_14, 0 }, { 1, arcs_13_15, 1 } };

static DFA dfa_13 = { "for_stmt", 16, states_13, first_13, 1 };

static Arc arcs_14_0[1] = { { 23, 1 } };

static Arc arcs_14_1[1] = { { 10, 2 } };

static Arc arcs_14_2[1] = { { 51, 3 } };

static Arc arcs_14_3[1] = { { 39, 4 } };

static Arc arcs_14_4[1] = { { 19, 5 } };

static Arc arcs_14_5[2] = { { 52, 6 }, { 0, 5 } };

static Arc arcs_14_6[1] = { { 19, 7 } };

static Arc arcs_14_7[1] = { { 0, 7 } };

static int first_14[1] = { 23 };

static DFAState states_14[8] = { { 1, arcs_14_0, 0 }, { 1, arcs_14_1, 0 }, { 1,
                                 arcs_14_2, 0 }, { 1, arcs_14_3, 0 }, { 1,
                                 arcs_14_4, 0 }, { 2, arcs_14_5, 1 }, { 1,
                                 arcs_14_6, 0 }, { 1, arcs_14_7, 1 } };

static DFA dfa_14 = { "if_stmt", 8, states_14, first_14, 1 };

static Arc arcs_15_0[3] = { { 11, 1 }, { 12, 1 }, { 15, 2 } };

static Arc arcs_15_1[1] = { { 15, 2 } };

static Arc arcs_15_2[1] = { { 0, 2 } };

static int first_15[3] = { 11, 12, 15 };

static DFAState states_15[3] = { { 3, arcs_15_0, 0 }, { 1, arcs_15_1, 0 }, { 1,
                                 arcs_15_2, 1 } };

static DFA dfa_15 = { "integer", 3, states_15, first_15, 3 };

static Arc arcs_16_0[2] = { { 13, 1 }, { 53, 1 } };

static Arc arcs_16_1[1] = { { 0, 1 } };

static int first_16[4] = { 11, 12, 13, 15 };

static DFAState states_16[2] = { { 2, arcs_16_0, 0 }, { 1, arcs_16_1, 1 } };

static DFA dfa_16 = { "konstant", 2, states_16, first_16, 4 };

static Arc arcs_17_0[1] = { { 35, 1 } };

static Arc arcs_17_1[1] = { { 14, 2 } };

static Arc arcs_17_2[2] = { { 9, 0 }, { 0, 2 } };

static int first_17[3] = { 4, 6, 7 };

static DFAState states_17[3] = { { 1, arcs_17_0, 0 }, { 1, arcs_17_1, 0 }, { 2,
                                 arcs_17_2, 1 } };

static DFA dfa_17 = { "paralist", 3, states_17, first_17, 3 };

static Arc arcs_18_0[1] = { { 26, 1 } };

static Arc arcs_18_1[1] = { { 10, 2 } };

static Arc arcs_18_2[1] = { { 14, 3 } };

static Arc arcs_18_3[2] = { { 39, 4 }, { 9, 2 } };

static Arc arcs_18_4[1] = { { 0, 4 } };

static int first_18[1] = { 26 };

static DFAState states_18[5] = { { 1, arcs_18_0, 0 }, { 1, arcs_18_1, 0 }, { 1,
                                 arcs_18_2, 0 }, { 2, arcs_18_3, 0 }, { 1,
                                 arcs_18_4, 1 } };

static DFA dfa_18 = { "read_stmt", 5, states_18, first_18, 1 };

static Arc arcs_19_0[1] = { { 25, 1 } };

static Arc arcs_19_1[2] = { { 10, 2 }, { 0, 1 } };

static Arc arcs_19_2[1] = { { 8, 3 } };

static Arc arcs_19_3[1] = { { 39, 4 } };

static Arc arcs_19_4[1] = { { 0, 4 } };

static int first_19[1] = { 25 };

static DFAState states_19[5] = { { 1, arcs_19_0, 0 }, { 2, arcs_19_1, 1 }, { 1,
                                 arcs_19_2, 0 }, { 1, arcs_19_3, 0 }, { 1,
                                 arcs_19_4, 1 } };

static DFA dfa_19 = { "return_stmt", 5, states_19, first_19, 1 };

static Arc arcs_20_0[9] = { { 21, 1 }, { 28, 2 }, { 14, 3 }, { 54, 1 }, { 55, 1
                            }, { 56, 4 }, { 57, 4 }, { 58, 1 }, { 59, 4 } };

static Arc arcs_20_1[1] = { { 0, 1 } };

static Arc arcs_20_2[2] = { { 43, 1 }, { 19, 2 } };

static Arc arcs_20_3[1] = { { 60, 4 } };

static Arc arcs_20_4[1] = { { 21, 1 } };

static int first_20[9] = { 14, 21, 22, 23, 24, 25, 26, 27, 28 };

static DFAState states_20[5] = { { 9, arcs_20_0, 0 }, { 1, arcs_20_1, 1 }, { 2,
                                 arcs_20_2, 0 }, { 1, arcs_20_3, 0 }, { 1,
                                 arcs_20_4, 0 } };

static DFA dfa_20 = { "stmt", 5, states_20, first_20, 9 };

static Arc arcs_21_0[3] = { { 10, 1 }, { 37, 2 }, { 49, 3 } };

static Arc arcs_21_1[2] = { { 39, 4 }, { 50, 5 } };

static Arc arcs_21_2[1] = { { 8, 4 } };

static Arc arcs_21_3[1] = { { 37, 2 } };

static Arc arcs_21_4[1] = { { 0, 4 } };

static Arc arcs_21_5[1] = { { 39, 4 } };

static int first_21[3] = { 10, 44, 37 };

static DFAState states_21[6] = { { 3, arcs_21_0, 0 }, { 2, arcs_21_1, 0 }, { 1,
                                 arcs_21_2, 0 }, { 1, arcs_21_3, 0 }, { 1,
                                 arcs_21_4, 1 }, { 1, arcs_21_5, 0 } };

static DFA dfa_21 = { "stmt_trailer", 6, states_21, first_21, 3 };

static Arc arcs_22_0[1] = { { 44, 1 } };

static Arc arcs_22_1[1] = { { 8, 2 } };

static Arc arcs_22_2[1] = { { 61, 3 } };

static Arc arcs_22_3[1] = { { 0, 3 } };

static int first_22[1] = { 44 };

static DFAState states_22[4] = { { 1, arcs_22_0, 0 }, { 1, arcs_22_1, 0 }, { 1,
                                 arcs_22_2, 0 }, { 1, arcs_22_3, 1 } };

static DFA dfa_22 = { "subscript", 4, states_22, first_22, 1 };

static Arc arcs_23_0[1] = { { 44, 1 } };

static Arc arcs_23_1[1] = { { 15, 2 } };

static Arc arcs_23_2[1] = { { 61, 3 } };

static Arc arcs_23_3[1] = { { 0, 3 } };

static int first_23[1] = { 44 };

static DFAState states_23[4] = { { 1, arcs_23_0, 0 }, { 1, arcs_23_1, 0 }, { 1,
                                 arcs_23_2, 0 }, { 1, arcs_23_3, 1 } };

static DFA dfa_23 = { "subscript2", 4, states_23, first_23, 1 };

static Arc arcs_24_0[1] = { { 27, 1 } };

static Arc arcs_24_1[1] = { { 10, 2 } };

static Arc arcs_24_2[1] = { { 8, 3 } };

static Arc arcs_24_3[1] = { { 39, 4 } };

static Arc arcs_24_4[1] = { { 28, 5 } };

static Arc arcs_24_5[1] = { { 62, 6 } };

static Arc arcs_24_6[2] = { { 62, 6 }, { 63, 7 } };

static Arc arcs_24_7[1] = { { 43, 8 } };

static Arc arcs_24_8[1] = { { 0, 8 } };

static int first_24[1] = { 27 };

static DFAState states_24[9] = { { 1, arcs_24_0, 0 }, { 1, arcs_24_1, 0 }, { 1,
                                 arcs_24_2, 0 }, { 1, arcs_24_3, 0 }, { 1,
                                 arcs_24_4, 0 }, { 1, arcs_24_5, 0 }, { 2,
                                 arcs_24_6, 0 }, { 1, arcs_24_7, 0 }, { 1,
                                 arcs_24_8, 1 } };

static DFA dfa_24 = { "switch_stmt", 9, states_24, first_24, 1 };

static Arc arcs_25_0[1] = { { 64, 1 } };

static Arc arcs_25_1[3] = { { 65, 0 }, { 66, 0 }, { 0, 1 } };

static int first_25[4] = { 10, 13, 14, 15 };

static DFAState states_25[2] = { { 1, arcs_25_0, 0 }, { 3, arcs_25_1, 1 } };

static DFA dfa_25 = { "term", 2, states_25, first_25, 4 };

static Arc arcs_26_0[3] = { { 4, 1 }, { 6, 1 }, { 7, 1 } };

static Arc arcs_26_1[1] = { { 0, 1 } };

static int first_26[3] = { 4, 6, 7 };

static DFAState states_26[2] = { { 3, arcs_26_0, 0 }, { 1, arcs_26_1, 1 } };

static DFA dfa_26 = { "type_name", 2, states_26, first_26, 3 };

static Arc arcs_27_0[1] = { { 35, 1 } };

static Arc arcs_27_1[1] = { { 41, 2 } };

static Arc arcs_27_2[2] = { { 9, 1 }, { 21, 3 } };

static Arc arcs_27_3[1] = { { 0, 3 } };

static int first_27[3] = { 4, 6, 7 };

static DFAState states_27[4] = { { 1, arcs_27_0, 0 }, { 1, arcs_27_1, 0 }, { 2,
                                 arcs_27_2, 0 }, { 1, arcs_27_3, 1 } };

static DFA dfa_27 = { "var_decl", 4, states_27, first_27, 3 };

static Arc arcs_28_0[1] = { { 14, 1 } };

static Arc arcs_28_1[2] = { { 38, 2 }, { 0, 1 } };

static Arc arcs_28_2[1] = { { 0, 2 } };

static int first_28[1] = { 14 };

static DFAState states_28[3] = { { 1, arcs_28_0, 0 }, { 2, arcs_28_1, 1 }, { 1,
                                 arcs_28_2, 1 } };

static DFA dfa_28 = { "var_item", 3, states_28, first_28, 1 };

static Arc arcs_29_0[1] = { { 24, 1 } };

static Arc arcs_29_1[1] = { { 10, 2 } };

static Arc arcs_29_2[2] = { { 67, 3 }, { 8, 4 } };

static Arc arcs_29_3[2] = { { 39, 5 }, { 9, 6 } };

static Arc arcs_29_4[1] = { { 39, 5 } };

static Arc arcs_29_5[1] = { { 0, 5 } };

static Arc arcs_29_6[1] = { { 8, 4 } };

static int first_29[1] = { 24 };

static DFAState states_29[7] = { { 1, arcs_29_0, 0 }, { 1, arcs_29_1, 0 }, { 2,
                                 arcs_29_2, 0 }, { 2, arcs_29_3, 0 }, { 1,
                                 arcs_29_4, 0 }, { 1, arcs_29_5, 1 }, { 1,
                                 arcs_29_6, 0 } };

static DFA dfa_29 = { "write_stmt", 7, states_29, first_29, 1 };

static DFA *dfas[30] = { &dfa_0, &dfa_1, &dfa_2, &dfa_3, &dfa_4, &dfa_5,
                         &dfa_6, &dfa_7, &dfa_8, &dfa_9, &dfa_10, &dfa_11,
                         &dfa_12, &dfa_13, &dfa_14, &dfa_15, &dfa_16, &dfa_17,
                         &dfa_18, &dfa_19, &dfa_20, &dfa_21, &dfa_22, &dfa_23,
                         &dfa_24, &dfa_25, &dfa_26, &dfa_27, &dfa_28, &dfa_29 };

Grammar CompilerGrammar = { 256, 30, 68, labels, dfas };

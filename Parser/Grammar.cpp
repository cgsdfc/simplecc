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

const char *SymbolNames[27] = {
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
    "subscript",
    "subscript2",
    "term",
    "type_name",
    "var_decl",
    "var_item",
    "while_stmt",
    "write_stmt",
};

static Label labels[63] = {
    {0, "EMPTY"},
    {3, 0},
    {260, 0},
    {263, 0},
    {0, "char"},
    {0, "const"},
    {0, "int"},
    {0, "void"},
    {1, "("},
    {264, 0},
    {1, ")"},
    {1, ","},
    {274, 0},
    {279, 0},
    {1, "+"},
    {1, "-"},
    {1, ";"},
    {0, "for"},
    {0, "if"},
    {0, "printf"},
    {0, "return"},
    {0, "scanf"},
    {0, "while"},
    {1, "{"},
    {4, 0},
    {0, 0},
    {5, 0},
    {1, "!="},
    {1, "<"},
    {1, "<="},
    {1, "=="},
    {1, ">"},
    {1, ">="},
    {278, 0},
    {261, 0},
    {1, "="},
    {270, 0},
    {271, 0},
    {276, 0},
    {280, 0},
    {258, 0},
    {1, "}"},
    {1, "["},
    {0, "main"},
    {262, 0},
    {277, 0},
    {266, 0},
    {257, 0},
    {275, 0},
    {259, 0},
    {0, "else"},
    {269, 0},
    {267, 0},
    {268, 0},
    {272, 0},
    {273, 0},
    {281, 0},
    {282, 0},
    {1, "]"},
    {265, 0},
    {1, "*"},
    {1, "/"},
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

static Arc arcs_1_1[1] = { { 9, 2 } };

static Arc arcs_1_2[2] = { { 10, 3 }, { 11, 1 } };

static Arc arcs_1_3[1] = { { 0, 3 } };

static int first_1[1] = { 8 };

static DFAState states_1[4] = { { 1, arcs_1_0, 0 }, { 1, arcs_1_1, 0 }, { 2,
                                arcs_1_2, 0 }, { 1, arcs_1_3, 1 } };

static DFA dfa_1 = { "arglist", 4, states_1, first_1, 1 };

static Arc arcs_2_0[4] = { { 2, 0 }, { 12, 1 }, { 13, 2 }, { 0, 0 } };

static Arc arcs_2_1[2] = { { 12, 1 }, { 0, 1 } };

static Arc arcs_2_2[3] = { { 12, 1 }, { 13, 2 }, { 0, 2 } };

static int first_2[18] = { 4, 5, 6, 7, 8, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                           23, 24, 25, 26 };

static DFAState states_2[3] = { { 4, arcs_2_0, 1 }, { 2, arcs_2_1, 1 }, { 3,
                                arcs_2_2, 1 } };

static DFA dfa_2 = { "compound_stmt", 3, states_2, first_2, 18 };

static Arc arcs_3_0[1] = { { 9, 1 } };

static Arc arcs_3_1[7] = { { 27, 2 }, { 28, 2 }, { 29, 2 }, { 30, 2 }, { 31, 2
                           }, { 32, 2 }, { 0, 1 } };

static Arc arcs_3_2[1] = { { 9, 3 } };

static Arc arcs_3_3[1] = { { 0, 3 } };

static int first_3[6] = { 8, 14, 15, 24, 25, 26 };

static DFAState states_3[4] = { { 1, arcs_3_0, 0 }, { 7, arcs_3_1, 1 }, { 1,
                                arcs_3_2, 0 }, { 1, arcs_3_3, 1 } };

static DFA dfa_3 = { "condition", 4, states_3, first_3, 6 };

static Arc arcs_4_0[1] = { { 5, 1 } };

static Arc arcs_4_1[1] = { { 33, 2 } };

static Arc arcs_4_2[1] = { { 34, 3 } };

static Arc arcs_4_3[2] = { { 11, 2 }, { 16, 4 } };

static Arc arcs_4_4[1] = { { 0, 4 } };

static int first_4[1] = { 5 };

static DFAState states_4[5] = { { 1, arcs_4_0, 0 }, { 1, arcs_4_1, 0 }, { 1,
                                arcs_4_2, 0 }, { 2, arcs_4_3, 0 }, { 1,
                                arcs_4_4, 1 } };

static DFA dfa_4 = { "const_decl", 5, states_4, first_4, 1 };

static Arc arcs_5_0[1] = { { 25, 1 } };

static Arc arcs_5_1[1] = { { 35, 2 } };

static Arc arcs_5_2[1] = { { 36, 3 } };

static Arc arcs_5_3[1] = { { 0, 3 } };

static int first_5[1] = { 25 };

static DFAState states_5[4] = { { 1, arcs_5_0, 0 }, { 1, arcs_5_1, 0 }, { 1,
                                arcs_5_2, 0 }, { 1, arcs_5_3, 1 } };

static DFA dfa_5 = { "const_item", 4, states_5, first_5, 1 };

static Arc arcs_6_0[5] = { { 11, 1 }, { 16, 2 }, { 23, 3 }, { 37, 4 }, { 38, 5
                           } };

static Arc arcs_6_1[1] = { { 39, 5 } };

static Arc arcs_6_2[1] = { { 0, 2 } };

static Arc arcs_6_3[1] = { { 40, 6 } };

static Arc arcs_6_4[1] = { { 23, 3 } };

static Arc arcs_6_5[2] = { { 11, 1 }, { 16, 2 } };

static Arc arcs_6_6[1] = { { 41, 2 } };

static int first_6[5] = { 8, 42, 11, 16, 23 };

static DFAState states_6[7] = { { 5, arcs_6_0, 0 }, { 1, arcs_6_1, 0 }, { 1,
                                arcs_6_2, 1 }, { 1, arcs_6_3, 0 }, { 1,
                                arcs_6_4, 0 }, { 2, arcs_6_5, 0 }, { 1,
                                arcs_6_6, 0 } };

static DFA dfa_6 = { "decl_trailer", 7, states_6, first_6, 5 };

static Arc arcs_7_0[1] = { { 33, 1 } };

static Arc arcs_7_1[2] = { { 43, 2 }, { 25, 3 } };

static Arc arcs_7_2[1] = { { 8, 4 } };

static Arc arcs_7_3[1] = { { 44, 5 } };

static Arc arcs_7_4[1] = { { 10, 6 } };

static Arc arcs_7_5[1] = { { 0, 5 } };

static Arc arcs_7_6[1] = { { 23, 7 } };

static Arc arcs_7_7[1] = { { 40, 8 } };

static Arc arcs_7_8[1] = { { 41, 5 } };

static int first_7[3] = { 4, 6, 7 };

static DFAState states_7[9] = { { 1, arcs_7_0, 0 }, { 2, arcs_7_1, 0 }, { 1,
                                arcs_7_2, 0 }, { 1, arcs_7_3, 0 }, { 1,
                                arcs_7_4, 0 }, { 1, arcs_7_5, 1 }, { 1,
                                arcs_7_6, 0 }, { 1, arcs_7_7, 0 }, { 1,
                                arcs_7_8, 0 } };

static DFA dfa_7 = { "declaration", 9, states_7, first_7, 3 };

static Arc arcs_8_0[3] = { { 14, 1 }, { 15, 1 }, { 45, 2 } };

static Arc arcs_8_1[1] = { { 45, 2 } };

static Arc arcs_8_2[3] = { { 14, 1 }, { 15, 1 }, { 0, 2 } };

static int first_8[6] = { 8, 14, 15, 24, 25, 26 };

static DFAState states_8[3] = { { 3, arcs_8_0, 0 }, { 1, arcs_8_1, 0 }, { 3,
                                arcs_8_2, 1 } };

static DFA dfa_8 = { "expr", 3, states_8, first_8, 6 };

static Arc arcs_9_0[4] = { { 8, 1 }, { 24, 2 }, { 25, 3 }, { 26, 2 } };

static Arc arcs_9_1[1] = { { 9, 4 } };

static Arc arcs_9_2[1] = { { 0, 2 } };

static Arc arcs_9_3[2] = { { 46, 2 }, { 0, 3 } };

static Arc arcs_9_4[1] = { { 10, 2 } };

static int first_9[4] = { 8, 24, 26, 25 };

static DFAState states_9[5] = { { 4, arcs_9_0, 0 }, { 1, arcs_9_1, 0 }, { 1,
                                arcs_9_2, 1 }, { 2, arcs_9_3, 1 }, { 1,
                                arcs_9_4, 0 } };

static DFA dfa_9 = { "factor", 5, states_9, first_9, 4 };

static Arc arcs_10_0[2] = { { 47, 1 }, { 48, 1 } };

static Arc arcs_10_1[1] = { { 0, 1 } };

static int first_10[2] = { 8, 42 };

static DFAState states_10[2] = { { 2, arcs_10_0, 0 }, { 1, arcs_10_1, 1 } };

static DFA dfa_10 = { "factor_trailer", 2, states_10, first_10, 2 };

static Arc arcs_11_0[1] = { { 17, 1 } };

static Arc arcs_11_1[1] = { { 8, 2 } };

static Arc arcs_11_2[1] = { { 25, 3 } };

static Arc arcs_11_3[1] = { { 35, 4 } };

static Arc arcs_11_4[1] = { { 9, 5 } };

static Arc arcs_11_5[1] = { { 16, 6 } };

static Arc arcs_11_6[1] = { { 49, 7 } };

static Arc arcs_11_7[1] = { { 16, 8 } };

static Arc arcs_11_8[1] = { { 25, 9 } };

static Arc arcs_11_9[1] = { { 35, 10 } };

static Arc arcs_11_10[1] = { { 25, 11 } };

static Arc arcs_11_11[2] = { { 14, 12 }, { 15, 12 } };

static Arc arcs_11_12[1] = { { 26, 13 } };

static Arc arcs_11_13[1] = { { 10, 14 } };

static Arc arcs_11_14[1] = { { 12, 15 } };

static Arc arcs_11_15[1] = { { 0, 15 } };

static int first_11[1] = { 17 };

static DFAState states_11[16] = { { 1, arcs_11_0, 0 }, { 1, arcs_11_1, 0 }, {
                                  1, arcs_11_2, 0 }, { 1, arcs_11_3, 0 }, { 1,
                                  arcs_11_4, 0 }, { 1, arcs_11_5, 0 }, { 1,
                                  arcs_11_6, 0 }, { 1, arcs_11_7, 0 }, { 1,
                                  arcs_11_8, 0 }, { 1, arcs_11_9, 0 }, { 1,
                                  arcs_11_10, 0 }, { 2, arcs_11_11, 0 }, { 1,
                                  arcs_11_12, 0 }, { 1, arcs_11_13, 0 }, { 1,
                                  arcs_11_14, 0 }, { 1, arcs_11_15, 1 } };

static DFA dfa_11 = { "for_stmt", 16, states_11, first_11, 1 };

static Arc arcs_12_0[1] = { { 18, 1 } };

static Arc arcs_12_1[1] = { { 8, 2 } };

static Arc arcs_12_2[1] = { { 49, 3 } };

static Arc arcs_12_3[1] = { { 10, 4 } };

static Arc arcs_12_4[1] = { { 12, 5 } };

static Arc arcs_12_5[2] = { { 50, 6 }, { 0, 5 } };

static Arc arcs_12_6[1] = { { 12, 7 } };

static Arc arcs_12_7[1] = { { 0, 7 } };

static int first_12[1] = { 18 };

static DFAState states_12[8] = { { 1, arcs_12_0, 0 }, { 1, arcs_12_1, 0 }, { 1,
                                 arcs_12_2, 0 }, { 1, arcs_12_3, 0 }, { 1,
                                 arcs_12_4, 0 }, { 2, arcs_12_5, 1 }, { 1,
                                 arcs_12_6, 0 }, { 1, arcs_12_7, 1 } };

static DFA dfa_12 = { "if_stmt", 8, states_12, first_12, 1 };

static Arc arcs_13_0[3] = { { 14, 1 }, { 15, 1 }, { 26, 2 } };

static Arc arcs_13_1[1] = { { 26, 2 } };

static Arc arcs_13_2[1] = { { 0, 2 } };

static int first_13[3] = { 26, 14, 15 };

static DFAState states_13[3] = { { 3, arcs_13_0, 0 }, { 1, arcs_13_1, 0 }, { 1,
                                 arcs_13_2, 1 } };

static DFA dfa_13 = { "integer", 3, states_13, first_13, 3 };

static Arc arcs_14_0[2] = { { 24, 1 }, { 51, 1 } };

static Arc arcs_14_1[1] = { { 0, 1 } };

static int first_14[4] = { 24, 26, 14, 15 };

static DFAState states_14[2] = { { 2, arcs_14_0, 0 }, { 1, arcs_14_1, 1 } };

static DFA dfa_14 = { "konstant", 2, states_14, first_14, 4 };

static Arc arcs_15_0[1] = { { 8, 1 } };

static Arc arcs_15_1[1] = { { 33, 2 } };

static Arc arcs_15_2[1] = { { 25, 3 } };

static Arc arcs_15_3[2] = { { 10, 4 }, { 11, 1 } };

static Arc arcs_15_4[1] = { { 0, 4 } };

static int first_15[1] = { 8 };

static DFAState states_15[5] = { { 1, arcs_15_0, 0 }, { 1, arcs_15_1, 0 }, { 1,
                                 arcs_15_2, 0 }, { 2, arcs_15_3, 0 }, { 1,
                                 arcs_15_4, 1 } };

static DFA dfa_15 = { "paralist", 5, states_15, first_15, 1 };

static Arc arcs_16_0[1] = { { 21, 1 } };

static Arc arcs_16_1[1] = { { 8, 2 } };

static Arc arcs_16_2[1] = { { 25, 3 } };

static Arc arcs_16_3[2] = { { 10, 4 }, { 11, 2 } };

static Arc arcs_16_4[1] = { { 0, 4 } };

static int first_16[1] = { 21 };

static DFAState states_16[5] = { { 1, arcs_16_0, 0 }, { 1, arcs_16_1, 0 }, { 1,
                                 arcs_16_2, 0 }, { 2, arcs_16_3, 0 }, { 1,
                                 arcs_16_4, 1 } };

static DFA dfa_16 = { "read_stmt", 5, states_16, first_16, 1 };

static Arc arcs_17_0[1] = { { 20, 1 } };

static Arc arcs_17_1[2] = { { 8, 2 }, { 0, 1 } };

static Arc arcs_17_2[1] = { { 9, 3 } };

static Arc arcs_17_3[1] = { { 10, 4 } };

static Arc arcs_17_4[1] = { { 0, 4 } };

static int first_17[1] = { 20 };

static DFAState states_17[5] = { { 1, arcs_17_0, 0 }, { 2, arcs_17_1, 1 }, { 1,
                                 arcs_17_2, 0 }, { 1, arcs_17_3, 0 }, { 1,
                                 arcs_17_4, 1 } };

static DFA dfa_17 = { "return_stmt", 5, states_17, first_17, 1 };

static Arc arcs_18_0[9] = { { 16, 1 }, { 23, 2 }, { 9, 3 }, { 52, 1 }, { 53, 1
                            }, { 54, 4 }, { 55, 4 }, { 56, 1 }, { 57, 4 } };

static Arc arcs_18_1[1] = { { 0, 1 } };

static Arc arcs_18_2[2] = { { 41, 1 }, { 12, 2 } };

static Arc arcs_18_3[2] = { { 16, 1 }, { 35, 5 } };

static Arc arcs_18_4[1] = { { 16, 1 } };

static Arc arcs_18_5[1] = { { 9, 4 } };

static int first_18[14] = { 8, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                            26 };

static DFAState states_18[6] = { { 9, arcs_18_0, 0 }, { 1, arcs_18_1, 1 }, { 2,
                                 arcs_18_2, 0 }, { 2, arcs_18_3, 0 }, { 1,
                                 arcs_18_4, 0 }, { 1, arcs_18_5, 0 } };

static DFA dfa_18 = { "stmt", 6, states_18, first_18, 14 };

static Arc arcs_19_0[1] = { { 42, 1 } };

static Arc arcs_19_1[1] = { { 9, 2 } };

static Arc arcs_19_2[1] = { { 58, 3 } };

static Arc arcs_19_3[1] = { { 0, 3 } };

static int first_19[1] = { 42 };

static DFAState states_19[4] = { { 1, arcs_19_0, 0 }, { 1, arcs_19_1, 0 }, { 1,
                                 arcs_19_2, 0 }, { 1, arcs_19_3, 1 } };

static DFA dfa_19 = { "subscript", 4, states_19, first_19, 1 };

static Arc arcs_20_0[1] = { { 42, 1 } };

static Arc arcs_20_1[1] = { { 26, 2 } };

static Arc arcs_20_2[1] = { { 58, 3 } };

static Arc arcs_20_3[1] = { { 0, 3 } };

static int first_20[1] = { 42 };

static DFAState states_20[4] = { { 1, arcs_20_0, 0 }, { 1, arcs_20_1, 0 }, { 1,
                                 arcs_20_2, 0 }, { 1, arcs_20_3, 1 } };

static DFA dfa_20 = { "subscript2", 4, states_20, first_20, 1 };

static Arc arcs_21_0[1] = { { 59, 1 } };

static Arc arcs_21_1[3] = { { 60, 0 }, { 61, 0 }, { 0, 1 } };

static int first_21[4] = { 8, 24, 26, 25 };

static DFAState states_21[2] = { { 1, arcs_21_0, 0 }, { 3, arcs_21_1, 1 } };

static DFA dfa_21 = { "term", 2, states_21, first_21, 4 };

static Arc arcs_22_0[3] = { { 4, 1 }, { 6, 1 }, { 7, 1 } };

static Arc arcs_22_1[1] = { { 0, 1 } };

static int first_22[3] = { 4, 6, 7 };

static DFAState states_22[2] = { { 3, arcs_22_0, 0 }, { 1, arcs_22_1, 1 } };

static DFA dfa_22 = { "type_name", 2, states_22, first_22, 3 };

static Arc arcs_23_0[1] = { { 33, 1 } };

static Arc arcs_23_1[1] = { { 39, 2 } };

static Arc arcs_23_2[2] = { { 11, 1 }, { 16, 3 } };

static Arc arcs_23_3[1] = { { 0, 3 } };

static int first_23[3] = { 4, 6, 7 };

static DFAState states_23[4] = { { 1, arcs_23_0, 0 }, { 1, arcs_23_1, 0 }, { 2,
                                 arcs_23_2, 0 }, { 1, arcs_23_3, 1 } };

static DFA dfa_23 = { "var_decl", 4, states_23, first_23, 3 };

static Arc arcs_24_0[1] = { { 25, 1 } };

static Arc arcs_24_1[2] = { { 38, 2 }, { 0, 1 } };

static Arc arcs_24_2[1] = { { 0, 2 } };

static int first_24[1] = { 25 };

static DFAState states_24[3] = { { 1, arcs_24_0, 0 }, { 2, arcs_24_1, 1 }, { 1,
                                 arcs_24_2, 1 } };

static DFA dfa_24 = { "var_item", 3, states_24, first_24, 1 };

static Arc arcs_25_0[1] = { { 22, 1 } };

static Arc arcs_25_1[1] = { { 8, 2 } };

static Arc arcs_25_2[1] = { { 49, 3 } };

static Arc arcs_25_3[1] = { { 10, 4 } };

static Arc arcs_25_4[1] = { { 12, 5 } };

static Arc arcs_25_5[1] = { { 0, 5 } };

static int first_25[1] = { 22 };

static DFAState states_25[6] = { { 1, arcs_25_0, 0 }, { 1, arcs_25_1, 0 }, { 1,
                                 arcs_25_2, 0 }, { 1, arcs_25_3, 0 }, { 1,
                                 arcs_25_4, 0 }, { 1, arcs_25_5, 1 } };

static DFA dfa_25 = { "while_stmt", 6, states_25, first_25, 1 };

static Arc arcs_26_0[1] = { { 19, 1 } };

static Arc arcs_26_1[1] = { { 8, 2 } };

static Arc arcs_26_2[2] = { { 62, 3 }, { 9, 4 } };

static Arc arcs_26_3[2] = { { 10, 5 }, { 11, 6 } };

static Arc arcs_26_4[1] = { { 10, 5 } };

static Arc arcs_26_5[1] = { { 0, 5 } };

static Arc arcs_26_6[1] = { { 9, 4 } };

static int first_26[1] = { 19 };

static DFAState states_26[7] = { { 1, arcs_26_0, 0 }, { 1, arcs_26_1, 0 }, { 2,
                                 arcs_26_2, 0 }, { 2, arcs_26_3, 0 }, { 1,
                                 arcs_26_4, 0 }, { 1, arcs_26_5, 1 }, { 1,
                                 arcs_26_6, 0 } };

static DFA dfa_26 = { "write_stmt", 7, states_26, first_26, 1 };

static DFA *dfas[27] = { &dfa_0, &dfa_1, &dfa_2, &dfa_3, &dfa_4, &dfa_5,
                         &dfa_6, &dfa_7, &dfa_8, &dfa_9, &dfa_10, &dfa_11,
                         &dfa_12, &dfa_13, &dfa_14, &dfa_15, &dfa_16, &dfa_17,
                         &dfa_18, &dfa_19, &dfa_20, &dfa_21, &dfa_22, &dfa_23,
                         &dfa_24, &dfa_25, &dfa_26 };

Grammar CompilerGrammar = { 256, 27, 63, labels, dfas };

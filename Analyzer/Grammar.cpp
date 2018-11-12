#include "Grammar.h"

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
    "flow_stmt",
    "for_stmt",
    "if_stmt",
    "integer",
    "paralist",
    "read_stmt",
    "return_stmt",
    "stmt",
    "stmt_trailer",
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
    {1, "{"},
    {1, "}"},
    {274, 0},
    {279, 0},
    {1, "!="},
    {1, "<"},
    {1, "<="},
    {1, "=="},
    {1, ">"},
    {1, ">="},
    {1, "+"},
    {1, "-"},
    {4, 0},
    {0, 0},
    {5, 0},
    {278, 0},
    {261, 0},
    {1, ";"},
    {1, "="},
    {270, 0},
    {258, 0},
    {271, 0},
    {276, 0},
    {280, 0},
    {1, "["},
    {0, "main"},
    {262, 0},
    {277, 0},
    {266, 0},
    {257, 0},
    {1, "]"},
    {268, 0},
    {269, 0},
    {272, 0},
    {273, 0},
    {281, 0},
    {282, 0},
    {0, "for"},
    {0, "if"},
    {0, "printf"},
    {0, "return"},
    {0, "scanf"},
    {0, "while"},
    {259, 0},
    {0, "else"},
    {267, 0},
    {275, 0},
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

static Arc arcs_2_0[1] = { { 12, 1 } };

static Arc arcs_2_1[4] = { { 13, 2 }, { 2, 1 }, { 14, 3 }, { 15, 4 } };

static Arc arcs_2_2[1] = { { 0, 2 } };

static Arc arcs_2_3[2] = { { 13, 2 }, { 14, 3 } };

static Arc arcs_2_4[3] = { { 13, 2 }, { 14, 3 }, { 15, 4 } };

static int first_2[1] = { 12 };

static DFAState states_2[5] = { { 1, arcs_2_0, 0 }, { 4, arcs_2_1, 0 }, { 1,
                                arcs_2_2, 1 }, { 2, arcs_2_3, 0 }, { 3,
                                arcs_2_4, 0 } };

static DFA dfa_2 = { "compound_stmt", 5, states_2, first_2, 1 };

static Arc arcs_3_0[1] = { { 9, 1 } };

static Arc arcs_3_1[7] = { { 16, 2 }, { 17, 2 }, { 18, 2 }, { 19, 2 }, { 20, 2
                           }, { 21, 2 }, { 0, 1 } };

static Arc arcs_3_2[1] = { { 9, 3 } };

static Arc arcs_3_3[1] = { { 0, 3 } };

static int first_3[6] = { 8, 22, 23, 24, 25, 26 };

static DFAState states_3[4] = { { 1, arcs_3_0, 0 }, { 7, arcs_3_1, 1 }, { 1,
                                arcs_3_2, 0 }, { 1, arcs_3_3, 1 } };

static DFA dfa_3 = { "condition", 4, states_3, first_3, 6 };

static Arc arcs_4_0[1] = { { 5, 1 } };

static Arc arcs_4_1[1] = { { 27, 2 } };

static Arc arcs_4_2[1] = { { 28, 3 } };

static Arc arcs_4_3[2] = { { 11, 2 }, { 29, 4 } };

static Arc arcs_4_4[1] = { { 0, 4 } };

static int first_4[1] = { 5 };

static DFAState states_4[5] = { { 1, arcs_4_0, 0 }, { 1, arcs_4_1, 0 }, { 1,
                                arcs_4_2, 0 }, { 2, arcs_4_3, 0 }, { 1,
                                arcs_4_4, 1 } };

static DFA dfa_4 = { "const_decl", 5, states_4, first_4, 1 };

static Arc arcs_5_0[1] = { { 25, 1 } };

static Arc arcs_5_1[1] = { { 30, 2 } };

static Arc arcs_5_2[2] = { { 24, 3 }, { 31, 3 } };

static Arc arcs_5_3[1] = { { 0, 3 } };

static int first_5[1] = { 25 };

static DFAState states_5[4] = { { 1, arcs_5_0, 0 }, { 1, arcs_5_1, 0 }, { 2,
                                arcs_5_2, 0 }, { 1, arcs_5_3, 1 } };

static DFA dfa_5 = { "const_item", 4, states_5, first_5, 1 };

static Arc arcs_6_0[5] = { { 11, 1 }, { 29, 2 }, { 32, 2 }, { 33, 3 }, { 34, 4
                           } };

static Arc arcs_6_1[1] = { { 35, 4 } };

static Arc arcs_6_2[1] = { { 0, 2 } };

static Arc arcs_6_3[1] = { { 32, 2 } };

static Arc arcs_6_4[2] = { { 11, 1 }, { 29, 2 } };

static int first_6[5] = { 36, 8, 11, 12, 29 };

static DFAState states_6[5] = { { 5, arcs_6_0, 0 }, { 1, arcs_6_1, 0 }, { 1,
                                arcs_6_2, 1 }, { 1, arcs_6_3, 0 }, { 2,
                                arcs_6_4, 0 } };

static DFA dfa_6 = { "decl_trailer", 5, states_6, first_6, 5 };

static Arc arcs_7_0[1] = { { 27, 1 } };

static Arc arcs_7_1[2] = { { 37, 2 }, { 25, 3 } };

static Arc arcs_7_2[1] = { { 8, 4 } };

static Arc arcs_7_3[1] = { { 38, 5 } };

static Arc arcs_7_4[1] = { { 10, 6 } };

static Arc arcs_7_5[1] = { { 0, 5 } };

static Arc arcs_7_6[1] = { { 32, 5 } };

static int first_7[3] = { 4, 6, 7 };

static DFAState states_7[7] = { { 1, arcs_7_0, 0 }, { 2, arcs_7_1, 0 }, { 1,
                                arcs_7_2, 0 }, { 1, arcs_7_3, 0 }, { 1,
                                arcs_7_4, 0 }, { 1, arcs_7_5, 1 }, { 1,
                                arcs_7_6, 0 } };

static DFA dfa_7 = { "declaration", 7, states_7, first_7, 3 };

static Arc arcs_8_0[3] = { { 22, 1 }, { 23, 1 }, { 39, 2 } };

static Arc arcs_8_1[1] = { { 39, 2 } };

static Arc arcs_8_2[3] = { { 22, 1 }, { 23, 1 }, { 0, 2 } };

static int first_8[6] = { 8, 22, 23, 24, 25, 26 };

static DFAState states_8[3] = { { 3, arcs_8_0, 0 }, { 1, arcs_8_1, 0 }, { 3,
                                arcs_8_2, 1 } };

static DFA dfa_8 = { "expr", 3, states_8, first_8, 6 };

static Arc arcs_9_0[4] = { { 8, 1 }, { 24, 2 }, { 25, 3 }, { 26, 2 } };

static Arc arcs_9_1[1] = { { 9, 4 } };

static Arc arcs_9_2[1] = { { 0, 2 } };

static Arc arcs_9_3[2] = { { 40, 2 }, { 0, 3 } };

static Arc arcs_9_4[1] = { { 10, 2 } };

static int first_9[4] = { 8, 24, 26, 25 };

static DFAState states_9[5] = { { 4, arcs_9_0, 0 }, { 1, arcs_9_1, 0 }, { 1,
                                arcs_9_2, 1 }, { 2, arcs_9_3, 1 }, { 1,
                                arcs_9_4, 0 } };

static DFA dfa_9 = { "factor", 5, states_9, first_9, 4 };

static Arc arcs_10_0[2] = { { 36, 1 }, { 41, 2 } };

static Arc arcs_10_1[1] = { { 9, 3 } };

static Arc arcs_10_2[1] = { { 0, 2 } };

static Arc arcs_10_3[1] = { { 42, 2 } };

static int first_10[2] = { 8, 36 };

static DFAState states_10[4] = { { 2, arcs_10_0, 0 }, { 1, arcs_10_1, 0 }, { 1,
                                 arcs_10_2, 1 }, { 1, arcs_10_3, 0 } };

static DFA dfa_10 = { "factor_trailer", 4, states_10, first_10, 2 };

static Arc arcs_11_0[6] = { { 43, 1 }, { 44, 1 }, { 45, 2 }, { 46, 2 }, { 47, 1
                            }, { 48, 2 } };

static Arc arcs_11_1[1] = { { 0, 1 } };

static Arc arcs_11_2[1] = { { 29, 1 } };

static int first_11[6] = { 49, 50, 51, 52, 53, 54 };

static DFAState states_11[3] = { { 6, arcs_11_0, 0 }, { 1, arcs_11_1, 1 }, { 1,
                                 arcs_11_2, 0 } };

static DFA dfa_11 = { "flow_stmt", 3, states_11, first_11, 6 };

static Arc arcs_12_0[1] = { { 49, 1 } };

static Arc arcs_12_1[1] = { { 8, 2 } };

static Arc arcs_12_2[1] = { { 25, 3 } };

static Arc arcs_12_3[1] = { { 30, 4 } };

static Arc arcs_12_4[1] = { { 9, 5 } };

static Arc arcs_12_5[1] = { { 29, 6 } };

static Arc arcs_12_6[1] = { { 55, 7 } };

static Arc arcs_12_7[1] = { { 29, 8 } };

static Arc arcs_12_8[1] = { { 25, 9 } };

static Arc arcs_12_9[1] = { { 30, 10 } };

static Arc arcs_12_10[1] = { { 25, 11 } };

static Arc arcs_12_11[2] = { { 22, 12 }, { 23, 12 } };

static Arc arcs_12_12[1] = { { 26, 13 } };

static Arc arcs_12_13[1] = { { 10, 14 } };

static Arc arcs_12_14[1] = { { 14, 15 } };

static Arc arcs_12_15[1] = { { 0, 15 } };

static int first_12[1] = { 49 };

static DFAState states_12[16] = { { 1, arcs_12_0, 0 }, { 1, arcs_12_1, 0 }, {
                                  1, arcs_12_2, 0 }, { 1, arcs_12_3, 0 }, { 1,
                                  arcs_12_4, 0 }, { 1, arcs_12_5, 0 }, { 1,
                                  arcs_12_6, 0 }, { 1, arcs_12_7, 0 }, { 1,
                                  arcs_12_8, 0 }, { 1, arcs_12_9, 0 }, { 1,
                                  arcs_12_10, 0 }, { 2, arcs_12_11, 0 }, { 1,
                                  arcs_12_12, 0 }, { 1, arcs_12_13, 0 }, { 1,
                                  arcs_12_14, 0 }, { 1, arcs_12_15, 1 } };

static DFA dfa_12 = { "for_stmt", 16, states_12, first_12, 1 };

static Arc arcs_13_0[1] = { { 50, 1 } };

static Arc arcs_13_1[1] = { { 8, 2 } };

static Arc arcs_13_2[1] = { { 55, 3 } };

static Arc arcs_13_3[1] = { { 10, 4 } };

static Arc arcs_13_4[1] = { { 14, 5 } };

static Arc arcs_13_5[2] = { { 56, 6 }, { 0, 5 } };

static Arc arcs_13_6[1] = { { 14, 7 } };

static Arc arcs_13_7[1] = { { 0, 7 } };

static int first_13[1] = { 50 };

static DFAState states_13[8] = { { 1, arcs_13_0, 0 }, { 1, arcs_13_1, 0 }, { 1,
                                 arcs_13_2, 0 }, { 1, arcs_13_3, 0 }, { 1,
                                 arcs_13_4, 0 }, { 2, arcs_13_5, 1 }, { 1,
                                 arcs_13_6, 0 }, { 1, arcs_13_7, 1 } };

static DFA dfa_13 = { "if_stmt", 8, states_13, first_13, 1 };

static Arc arcs_14_0[3] = { { 22, 1 }, { 23, 1 }, { 26, 2 } };

static Arc arcs_14_1[1] = { { 26, 2 } };

static Arc arcs_14_2[1] = { { 0, 2 } };

static int first_14[3] = { 26, 22, 23 };

static DFAState states_14[3] = { { 3, arcs_14_0, 0 }, { 1, arcs_14_1, 0 }, { 1,
                                 arcs_14_2, 1 } };

static DFA dfa_14 = { "integer", 3, states_14, first_14, 3 };

static Arc arcs_15_0[1] = { { 8, 1 } };

static Arc arcs_15_1[1] = { { 27, 2 } };

static Arc arcs_15_2[1] = { { 25, 3 } };

static Arc arcs_15_3[2] = { { 10, 4 }, { 11, 1 } };

static Arc arcs_15_4[1] = { { 0, 4 } };

static int first_15[1] = { 8 };

static DFAState states_15[5] = { { 1, arcs_15_0, 0 }, { 1, arcs_15_1, 0 }, { 1,
                                 arcs_15_2, 0 }, { 2, arcs_15_3, 0 }, { 1,
                                 arcs_15_4, 1 } };

static DFA dfa_15 = { "paralist", 5, states_15, first_15, 1 };

static Arc arcs_16_0[1] = { { 53, 1 } };

static Arc arcs_16_1[1] = { { 8, 2 } };

static Arc arcs_16_2[1] = { { 25, 3 } };

static Arc arcs_16_3[2] = { { 10, 4 }, { 11, 2 } };

static Arc arcs_16_4[1] = { { 0, 4 } };

static int first_16[1] = { 53 };

static DFAState states_16[5] = { { 1, arcs_16_0, 0 }, { 1, arcs_16_1, 0 }, { 1,
                                 arcs_16_2, 0 }, { 2, arcs_16_3, 0 }, { 1,
                                 arcs_16_4, 1 } };

static DFA dfa_16 = { "read_stmt", 5, states_16, first_16, 1 };

static Arc arcs_17_0[1] = { { 52, 1 } };

static Arc arcs_17_1[2] = { { 8, 2 }, { 0, 1 } };

static Arc arcs_17_2[1] = { { 9, 3 } };

static Arc arcs_17_3[1] = { { 10, 4 } };

static Arc arcs_17_4[1] = { { 0, 4 } };

static int first_17[1] = { 52 };

static DFAState states_17[5] = { { 1, arcs_17_0, 0 }, { 2, arcs_17_1, 1 }, { 1,
                                 arcs_17_2, 0 }, { 1, arcs_17_3, 0 }, { 1,
                                 arcs_17_4, 1 } };

static DFA dfa_17 = { "return_stmt", 5, states_17, first_17, 1 };

static Arc arcs_18_0[4] = { { 29, 1 }, { 12, 2 }, { 25, 3 }, { 57, 1 } };

static Arc arcs_18_1[1] = { { 0, 1 } };

static Arc arcs_18_2[2] = { { 13, 1 }, { 14, 2 } };

static Arc arcs_18_3[2] = { { 29, 1 }, { 58, 4 } };

static Arc arcs_18_4[1] = { { 29, 1 } };

static int first_18[9] = { 12, 49, 50, 51, 52, 53, 54, 25, 29 };

static DFAState states_18[5] = { { 4, arcs_18_0, 0 }, { 1, arcs_18_1, 1 }, { 2,
                                 arcs_18_2, 0 }, { 2, arcs_18_3, 0 }, { 1,
                                 arcs_18_4, 0 } };

static DFA dfa_18 = { "stmt", 5, states_18, first_18, 9 };

static Arc arcs_19_0[3] = { { 30, 1 }, { 36, 2 }, { 41, 3 } };

static Arc arcs_19_1[1] = { { 9, 3 } };

static Arc arcs_19_2[1] = { { 9, 4 } };

static Arc arcs_19_3[1] = { { 0, 3 } };

static Arc arcs_19_4[1] = { { 42, 5 } };

static Arc arcs_19_5[1] = { { 30, 1 } };

static int first_19[3] = { 8, 36, 30 };

static DFAState states_19[6] = { { 3, arcs_19_0, 0 }, { 1, arcs_19_1, 0 }, { 1,
                                 arcs_19_2, 0 }, { 1, arcs_19_3, 1 }, { 1,
                                 arcs_19_4, 0 }, { 1, arcs_19_5, 0 } };

static DFA dfa_19 = { "stmt_trailer", 6, states_19, first_19, 3 };

static Arc arcs_20_0[1] = { { 36, 1 } };

static Arc arcs_20_1[1] = { { 26, 2 } };

static Arc arcs_20_2[1] = { { 42, 3 } };

static Arc arcs_20_3[1] = { { 0, 3 } };

static int first_20[1] = { 36 };

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

static Arc arcs_23_0[1] = { { 27, 1 } };

static Arc arcs_23_1[1] = { { 35, 2 } };

static Arc arcs_23_2[2] = { { 11, 1 }, { 29, 3 } };

static Arc arcs_23_3[1] = { { 0, 3 } };

static int first_23[3] = { 4, 6, 7 };

static DFAState states_23[4] = { { 1, arcs_23_0, 0 }, { 1, arcs_23_1, 0 }, { 2,
                                 arcs_23_2, 0 }, { 1, arcs_23_3, 1 } };

static DFA dfa_23 = { "var_decl", 4, states_23, first_23, 3 };

static Arc arcs_24_0[1] = { { 25, 1 } };

static Arc arcs_24_1[2] = { { 34, 2 }, { 0, 1 } };

static Arc arcs_24_2[1] = { { 0, 2 } };

static int first_24[1] = { 25 };

static DFAState states_24[3] = { { 1, arcs_24_0, 0 }, { 2, arcs_24_1, 1 }, { 1,
                                 arcs_24_2, 1 } };

static DFA dfa_24 = { "var_item", 3, states_24, first_24, 1 };

static Arc arcs_25_0[1] = { { 54, 1 } };

static Arc arcs_25_1[1] = { { 8, 2 } };

static Arc arcs_25_2[1] = { { 55, 3 } };

static Arc arcs_25_3[1] = { { 10, 4 } };

static Arc arcs_25_4[1] = { { 14, 5 } };

static Arc arcs_25_5[1] = { { 0, 5 } };

static int first_25[1] = { 54 };

static DFAState states_25[6] = { { 1, arcs_25_0, 0 }, { 1, arcs_25_1, 0 }, { 1,
                                 arcs_25_2, 0 }, { 1, arcs_25_3, 0 }, { 1,
                                 arcs_25_4, 0 }, { 1, arcs_25_5, 1 } };

static DFA dfa_25 = { "while_stmt", 6, states_25, first_25, 1 };

static Arc arcs_26_0[1] = { { 51, 1 } };

static Arc arcs_26_1[1] = { { 8, 2 } };

static Arc arcs_26_2[2] = { { 62, 3 }, { 9, 4 } };

static Arc arcs_26_3[2] = { { 10, 5 }, { 11, 6 } };

static Arc arcs_26_4[1] = { { 10, 5 } };

static Arc arcs_26_5[1] = { { 0, 5 } };

static Arc arcs_26_6[1] = { { 9, 4 } };

static int first_26[1] = { 51 };

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

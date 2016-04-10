/*
   BATCH NUMBER: 23
   PRABODH AGARWAL 2012B1A7801P
   DEEPANSHU SINGH 2012B3A7593P

   parseDef.h: defines the variables and data structures used in parser.c
   */

#ifndef PARSERDEF
#define PARSERDEF

#include "lexer.h"

#define NON_TERMINAL_OFFSET 1000 // Offset for Non-Terminal's enum. Terminal Enum starts from 0.
#define gg (*g)
#define MAX_RULES 88 // Number of rules in Grammar
#define MAX_RULE_SIZE 100 // Maximum size of a rule
#define MAX_TERMINALS 57 // Number of Terminals in a grammar
#define MAX_NON_TERMINALS 52 // Number of Non-Terminals in a grammar


typedef enum{
    all = NON_TERMINAL_OFFSET,
    allVar,
    arithmeticExpression,
    assignmentStmt,
    booleanExpression,
    conditionalStmt,
    constructedDatatype,
    dataType,
    declaration,
    declarations,
    innerTerm,
    elseStmt,
    factor,
    fieldDefinition,
    fieldDefinitions,
    funCallStmt,
    function,
    global_or_not,
    mulDiv,
    idList,
    input_par,
    inputParameters,
    ioStmt,
    iterativeStmt,
    logicalOp,
    plusMinus,
    mainFunctions,
    more_ids,
    moreFields,
    operator,
    optionalReturn,
    otherFunctions,
    otherStmts,
    output_par,
    outputParameters,
    parameter_list,
    primitiveDatatype,
    program,
    relationalOp,
    remaining_list,
    returnStmt,
    singleOrRecId_,
    singleOrRecId,
    stmt,
    stmts,
    recAdjust,
    term,
    termPrime,
    typeDefinition,
    typeDefinitions,
    var
}nontermid;

//Data Structure to store Parse Tree
struct parseTree{
    union { //union used because a node can either be a non terminal or a terminal not both
        nontermid nonterm;
        tokenInfo term;
    };
    short isTerminal; // whether the node is a terminal or not
    struct parseTree *children; // to store pointers to the children of the node
    int nochildren; // number of children of a node
};

struct astree{
    union { //union used because a node can either be a non terminal or a terminal not both
        nontermid nonterm;
        tokenInfo term;
    };
    short isTerminal; // whether the node is a terminal or not
    struct astree *children; // to store pointers to the children of the node
    int nochildren; // number of children of a node
    int type; // used by ast for typechecking
    unsigned long long line_num;
};

typedef struct parseTree parseTree;
typedef struct astree astree;
#endif

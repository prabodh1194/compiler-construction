#ifndef PARSEDEF
#define PARSEDEF

#include "lexer.h"

#define NON_TERMINAL_OFFSET 12345
#define gg (*g)
#define MAX_RULES 90
#define MAX_RULE_SIZE 100
#define MAX_TERMINALS 57
#define MAX_NON_TERMINALS 52


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

struct parseTree{
    short isTerminal;
    union {
    nontermid nonterm;
    tokenInfo term;
    };
    struct parseTree *children;
    int nochildren;
};

typedef struct parseTree parseTree;
#endif

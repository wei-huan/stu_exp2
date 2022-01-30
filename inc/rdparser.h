#ifndef __RDPARSER_H
#define __RDPARSER_H

#include "man_lex.h"
#include <dirent.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_MODE 1
typedef struct _ast ast;
typedef struct _ast *past;

typedef struct _ndata {
    long int value;
    char dimen;   // dimension
	char type[10];
    char id[30];
} ndata, *pndata; // nodedata

struct _ast {
    ndata data;
    char *nodeType;
    past ls;
    past rs;
	past bro;
};

extern FILE *ana_file;
extern int nextToken;

past CompUnit(void);
past Decl(void);
past ConstDecl(void);
past ConstDef(void);
past VarDecl(void);
past VarDefSameLeftFactor(void);
past VarDef(void);
past FuncDef(void);
past BType(void);
past ConstInitVal(void);
past ConstExp(void);
past InitVal(void);
past FuncType(void);
past FuncFParams(void);
past FuncFParam(void);
past Block(void);
past BlockItem(void);
past Stmt(void);
past Exp(void);
past Cond(void);
past LVal(void);
past PrimaryExp(void);
past Number(void);
past UnaryExp(void);
past UnaryOp(void);
past FuncRParams(void);
past MulExp(void);
past AddExp(void);
past RelExp(void);
past EqExp(void);
past LAndExp(void);
past LOrExp(void);
past MulExp_Extend(void);
past AddExp_Extend(void);
past RelExp_Extend(void);
past EqExp_Extend(void);
past LAndExp_Extend(void);
past LOrExp_Extend(void);
past Recursive_Parse(void);
void ShowAst(past node, int nest);
void FreeAst(past node);
#endif /*__RDPARSER_H*/

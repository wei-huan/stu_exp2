#include "../inc/rdparser.h"

static int tok = 0;
void advance() { tok = lex_ana(); }

int save_recall_pos() {
    int recall_pos;

    if (last_get == 0)
        recall_pos = ftell(ana_file) - 2;
    else
        recall_pos = ftell(ana_file) - 1;

    if (recall_pos == -2)
        recall_pos = 0;

    return recall_pos;
}

void recall(int last_pos) {
    int idx2 = ftell(ana_file);
    int recall_len = idx2 - last_pos;

    fseek(ana_file, (-1 * recall_len), SEEK_CUR);
    first_get = 1;

    getChar();
}

void AstAppendBro(past *fstnode, past addnode) {
    if (!*fstnode) {
        *fstnode = addnode;
        return;
    }

    past s = *fstnode;

    while (s->bro)
        s = s->bro;

    s->bro = addnode;
}

past newAstNode() {
    past node = (past)malloc(sizeof(ast));
    if (!node) {
        printf("run out of memory.\n");
        exit(0);
    }
    memset(node, 0, sizeof(ast));
    return node;
}

past newType(char *typename) {
    past type = newAstNode();
    type->nodeType = "Type";
    strcpy(type->data.type, typename);
    return type;
}

past newNum(int value) {
    past num = newAstNode();
    num->nodeType = "Num";
    num->data.value = value;
    return num;
}

past newVar(char *name, int dimension, past dimen_list, past data_list) {
    past var = newAstNode();
    strcpy(var->data.id, name);
    var->nodeType = "Varible";
    var->data.dimen = dimension;
    var->ls = dimen_list;
    var->rs = data_list;
    return var;
}

past newConstVar(char *name, int dimension, past dimen_list, past data_list) {
    past var = newAstNode();
    strcpy(var->data.id, name);
    var->nodeType = "ConstVar";
    var->data.dimen = dimension;
    var->ls = dimen_list;
    var->rs = data_list;
    return var;
}

past newOpcode(char *op, past ls, past rs) {
    past opcode = newAstNode();
    opcode->nodeType = "Opcode";
    strcpy(opcode->data.type, op);
    opcode->ls = ls;
    opcode->rs = rs;
    return opcode;
}

past newFuncDef(past params, past block, char *id, char *rettype) {
    past funcdef = newAstNode();
    funcdef->nodeType = "FuncDef";
    strcpy(funcdef->data.id, id);
    strcpy(funcdef->data.type, rettype);
    funcdef->ls = params;
    funcdef->rs = block;
    return funcdef;
}

past newFuncFParam(char *type, char *id, int dimen,
                   past dimen_list) {
    past funcfparam = newAstNode();
    funcfparam->nodeType = "FuncFParam";
    funcfparam->data.dimen = dimen;
    strcpy(funcfparam->data.type, type);
    strcpy(funcfparam->data.id, id);
    funcfparam->ls = dimen_list;
    return funcfparam;
}

past newFuncRParam(past exp) {
    past funcfparam = newAstNode();
    funcfparam->nodeType = "FuncRParam";
    funcfparam->ls = exp;
    return funcfparam;
}

past newBlock_list(past decl_or_stmt, past block_list) {
    past newblock_list = newAstNode();
    newblock_list->nodeType = "Block_List";
    // newblock_list->ls = decl_or_stmt;
    // newblock_list->bro_rs = block_list;
    return newblock_list;
}

past newBlock(past block_list) {
    past block = newAstNode();
    block->nodeType = "Block";
    block->rs = block_list;
    return block;
}

past newInitVal(past exp_or_initval, past initvals) {
    past newinitval = newAstNode();
    // newinitval->ls = exp_or_initval;
    // newinitval->bro_rs = initvals;

    if (!initvals)
        newinitval->nodeType = "InitVal";
    else
        newinitval->nodeType = "InitVals";

    return newinitval;
}

past newConstDef_List(past constdef, past constdef_list) {
    past newconstdef_list = newAstNode();
    newconstdef_list->nodeType = "ConstDefList";
    // newconstdef_list->ls = constdef;
    // newconstdef_list->bro_rs = constdef_list;
    return constdef_list;
}

past newConstDecl(past type, past constdef_list) {
    past constdecl = newAstNode();
    constdecl->nodeType = "ConstDecl";
    strcpy(constdecl->data.type, type->data.type);
    constdecl->ls = constdef_list;
    return constdecl;
}

past newVarDecl(char* type, past var) {
    past vardecl = newAstNode();
    vardecl->nodeType = "VarDecl";
    strcpy(vardecl->data.type, type);
    vardecl->ls = var;
    return vardecl;
}

past newVarDef_List(past vardef, past vardef_list) {
    past newvardef_list = newAstNode();
    newvardef_list->nodeType = "VarDef_List";
    // newvardef_list->ls = vardef;
    // newvardef_list->bro_rs = vardef_list;
    return newvardef_list;
}

past newFuncCall(char *funcname, past funcrparams_list) {
    past funccall = newAstNode();
    strcpy(funccall->data.id, funcname);
    funccall->nodeType = "FuncCall";
    funccall->ls = funcrparams_list;
    return funccall;
}

past newWhileStmt(past cond, past stmt) {
    past whilestmt = newAstNode();
    whilestmt->nodeType = "WhileStmt";
    whilestmt->ls = cond;
    whilestmt->rs = stmt;
    return whilestmt;
}

past newContinueStmt() {
    past continuestmt = newAstNode();
    continuestmt->nodeType = "ContinueStmt";
    return continuestmt;
}

past newReturnStmt(past exp) {
    past returnstmt = newAstNode();
    returnstmt->nodeType = "ReturnStmt";
    returnstmt->ls = exp;
    return returnstmt;
}

past newBreakStmt() {
    past breakstmt = newAstNode();
    breakstmt->nodeType = "BreakStmt";
    return breakstmt;
}

past newIfStmt(past cond, past stmt, past else_stmt) {
    past ifstmt = newAstNode();
    ifstmt->nodeType = "IfStmt";
    ifstmt->ls = cond;
    ifstmt->rs = stmt;
    ifstmt->bro = else_stmt;
    return ifstmt;
}

past newElseStmt(past stmt) {
    past elsestmt = newAstNode();
    elsestmt->nodeType = "ElseStmt";
    elsestmt->ls = stmt;
    return elsestmt;
}

past newNullStmt() {
    past nullstmt = newAstNode();
    nullstmt->nodeType = "NullStmt";
    return nullstmt;
}

past CompUnit() {
    int recall_pos;
    past CompUnit_List = NULL;

    while (1) {
        recall_pos = save_recall_pos();
        advance();

        if (tok == tok_EOF)
            return CompUnit_List;
        else {
            recall(recall_pos);
            past funcdef = FuncDef();

            if (!funcdef) {
                recall(recall_pos);
                past decl = Decl();

                if (!decl)
                    return NULL;
                else
                    AstAppendBro(&CompUnit_List, decl);
            } else
                AstAppendBro(&CompUnit_List, funcdef);
        }
    }
}

past Decl() {
    int recall_pos = save_recall_pos();

    past constdecl = ConstDecl();
    if (constdecl)
        return constdecl;
    else {
        recall(recall_pos);
        return VarDecl();
    }
}

past ConstDecl() {
    past constdef_list = NULL;

    advance();
    if (tok == tok_CONST) {
        past btype = BType();
        if (btype) {
            past constdef = ConstDef();
            if (constdef) {

                AstAppendBro(&constdef_list, constdef);
                advance();

                if (tok == tok_COMMA) {
                    while (1) {
                        past constdef = ConstDef();
                        if (constdef) {

                            AstAppendBro(&constdef_list, constdef);
                            advance();

                            if (tok != tok_COMMA)
                                break;
                        } else {
                            return NULL;
                        }
                    }

                    if (tok == tok_SEMICOLON)
                        return newConstDecl(btype, constdef_list);
                    else
                        return NULL;
                } else if (tok == tok_SEMICOLON) {
                    return newConstDecl(btype, constdef_list);
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past ConstDef() {
    past dimen_list = NULL;
    advance();

    if (tok == tok_ID) {
        char id_str[10];
        strcpy(id_str, lex_str);

        advance();

        if (tok == tok_ASSIGN) {
            past constinitval = ConstInitVal();
            if (constinitval) {
                return newConstVar(id_str, 0, NULL, constinitval);
            } else {
                return NULL;
            }
        } else if (tok == tok_LSQUARE) {
            past constexp = ConstExp();
            if (constexp) {
                advance();

                if (tok == tok_RSQUARE) {
                    AstAppendBro(&dimen_list, constexp);
                    advance();

                    if (tok == tok_ASSIGN) {
                        past constinitval = ConstInitVal();

                        if (constinitval) {
                            return newVar(id_str, 1, dimen_list, constinitval);
                        } else {
                            return NULL;
                        }
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past BType() {
    advance();

    if (tok == tok_INT)
        return newType(lex_str);
    else
        return NULL;
}

past VarDecl() {
    past vardef_list = NULL;
    int last_pos = save_recall_pos();

    past btype = BType();
    if (btype) {
        past vardef = VarDef();
        if (vardef) {
            advance();

            if (tok == tok_SEMICOLON)
                return newVarDecl(btype->data.type, vardef);
            else {
                while (1) {
                    if (tok == tok_COMMA) {
                        past newvardef = VarDef();

                        if (newvardef){
                            AstAppendBro(&vardef, newvardef);
                            advance();
                        }
                        else
                            return NULL;
                    } else
                        break;
                }

                if (tok == tok_SEMICOLON) {
                    return newVarDecl(btype->data.type, vardef);
                } else {
                    return NULL;
                }
            }
        } else
            return NULL;
    } else
        return NULL;
}

past VarDef() {
    int dimension = 0;
    past dimen_list = NULL;

    advance();
    if (tok == tok_ID) {
        char id_str[100];
        strcpy(id_str, lex_str);

        int recall_pos = save_recall_pos();
        advance();

        if (tok == tok_ASSIGN) {
            past initval = InitVal();

            if (initval) {
                past var = newVar(id_str, dimension, NULL, initval);
                return var;
            } else
                return NULL;

        } else if (tok == tok_LSQUARE) {
            while (1) {
                if (tok == tok_LSQUARE) {
                    past constexp = ConstExp();
                    if (constexp) {
                        advance();

                        if (tok == tok_RSQUARE) {
                            dimension++;
                            AstAppendBro(&dimen_list, constexp);

                            recall_pos = save_recall_pos();
                            advance();
                        } else
                            return NULL;
                    } else
                        return NULL;
                } else
                    break;
            }

            past var = newVar(id_str, dimension, dimen_list, NULL);
            if (tok == tok_ASSIGN) {
                past initval = InitVal();
                if (initval) {
                    past var = newVar(id_str, dimension, dimen_list, initval);
                    return var;
                } else
                    return NULL;
            } else {
                recall(recall_pos);
                return var;
            }
        } else {
            recall(recall_pos);
            return newVar(id_str, 0, NULL, NULL);
        }
    } else
        return NULL;
}

past FuncDef() {
    int last_pos;

    past type = FuncType();
    if (type) {
        advance();

        if (tok == tok_ID) {
            char id_str[100];
            strcpy((char *)id_str, lex_str);

            advance();
            if (tok == tok_LPAR) {
                last_pos = save_recall_pos();
                advance();

                if (tok == tok_RPAR) {
                    past block = Block();
                    if (block)
                        return newFuncDef(NULL, block, id_str, type->data.type);
                    else
                        return NULL;
                } else {
                    recall(last_pos);

                    past funcfarams = FuncFParams();

                    if (funcfarams) {
                        advance();

                        if (tok == tok_RPAR) {
                            past block = Block();
                            if (block)
                                return newFuncDef(funcfarams, block, id_str, type->data.type);
                            else
                                return NULL;
                        } else {
                            return NULL;
                        }
                    } else {
                        return NULL;
                    }
                }
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past ConstInitVal() {
    past constinitval_list = NULL;
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_LBRACKET) {
        recall_pos = save_recall_pos();
        advance();

        if (tok == tok_RBRACKET) {
            return NULL;
        }

        recall(recall_pos);

        while (1) {
            past constinitval = ConstInitVal();
            if (constinitval) {
                AstAppendBro(&constinitval_list, constinitval);
                advance();

                if (tok == tok_COMMA) {
                    continue;
                } else if (tok == tok_RBRACKET) {
                    return constinitval_list;
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        }
    } else {
        recall(recall_pos);
        return ConstExp();
    }
}

past FuncType() {
    advance();
    if (tok == tok_VOID || tok == tok_INT)
        return newType(lex_str);
    else
        return NULL;
}

past FuncFParams() {
    int recall_pos;

    past funcfparams = NULL;

    while (1) {
        past funcfparam = FuncFParam();

        if (funcfparam) {
            AstAppendBro(&funcfparams, funcfparam);
            recall_pos = save_recall_pos();
            advance();

            if (tok != tok_COMMA) {
                recall(recall_pos);
                return funcfparams;
            }
        } else
            return NULL;
    }
}

past FuncFParam() {
    int dimension = 0;
    past dimen_list = NULL;
    int recall_pos;

    if (BType()) {
        char type_str[10];
        strcpy(type_str, lex_str);

        advance();

        if (tok == tok_ID) {
            char id_str[30];
            strcpy(id_str, lex_str);

            recall_pos = save_recall_pos();
            advance();

            if (tok == tok_LSQUARE) {
                advance();

                if (tok == tok_RSQUARE) {
                    dimension++;

                    while (1) {
                        recall_pos = save_recall_pos();
                        advance();

                        if (tok == tok_LSQUARE) {
                            past exp = Exp();

                            if (exp) {
                                advance();

                                if (tok == tok_RSQUARE) {
                                    dimension++;
                                    AstAppendBro(&dimen_list, exp);
                                    continue;
                                } else {
                                    return NULL;
                                }
                            } else {
                                return NULL;
                            }
                        } else {
                            recall(recall_pos);

                            past funcfparam = newFuncFParam(type_str, id_str, dimension, dimen_list);
                            return funcfparam;
                        }
                    }
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);
                return newFuncFParam(type_str, id_str, dimension, NULL);
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past Block() {
    int recall_pos;
    past block_list = NULL;
    advance();

    if (tok == tok_LBRACKET) {
        recall_pos = save_recall_pos();

        advance();

        if (tok == tok_RBRACKET) {
            return newBlock(NULL);
        } else {
            recall(recall_pos);

            while (1) {
                past blockitem = BlockItem();
                if (blockitem) {
                    AstAppendBro(&block_list, blockitem);
                    recall_pos = save_recall_pos();
                    advance();

                    if (tok == tok_RBRACKET)
                        return newBlock(block_list);
                    else
                        recall(recall_pos);
                } else
                    return NULL;
            }
        }
    } else
        return NULL;
}

past BlockItem() {
    int recall_pos = save_recall_pos();

    past stmt = Stmt();
    if (stmt)
        return stmt;
    else{
        recall(recall_pos);
        return Decl();
    }
}

past Stmt() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_IF) {
        advance();

        if (tok == tok_LPAR) {
            past cond = Cond();
            if (cond) {
                advance();

                if (tok == tok_RPAR) {
                    past stmt = Stmt();
                    if (stmt) {

                        recall_pos = save_recall_pos();
                        advance();

                        if (tok == tok_ELSE) {
                            past else_stmt = Stmt();
                            if (else_stmt) {
                                past ifstmt = newIfStmt(cond, stmt, newElseStmt(else_stmt));
                                return ifstmt;
                            } else {
                                return NULL;
                            }
                        } else {
                            recall(recall_pos);

                            past ifstmt = newIfStmt(cond, stmt, NULL);
                            return ifstmt;
                        }
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else if (tok == tok_WHILE) {
        advance();

        if (tok == tok_LPAR) {
            past cond = Cond();
            if (cond) {
                advance();
                if (tok == tok_RPAR) {
                    past stmt = Stmt();
                    if (stmt) {
                        past while_stmt = newWhileStmt(cond, stmt);

                        return while_stmt;
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else if (tok == tok_BREAK) {
        advance();

        if (tok == tok_SEMICOLON) {
            return newBreakStmt();
        } else {
            return NULL;
        }
    } else if (tok == tok_CONTINUE) {
        advance();

        if (tok == tok_SEMICOLON) {
            return newContinueStmt();
        } else {
            return NULL;
        }
    } else if (tok == tok_RETURN) {
        recall_pos = save_recall_pos();
        advance();

        if (tok == tok_SEMICOLON) {
            past returnstmt = newReturnStmt(NULL);

            return returnstmt;
        }

        recall(recall_pos);

        past exp = Exp();
        if (exp) {
            advance();

            if (tok == tok_SEMICOLON) {
                past returnstmt = newReturnStmt(exp);
                return returnstmt;
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else if (tok == tok_SEMICOLON) {
        past nullstmt = newNullStmt();

        return nullstmt;
    } else {
        recall(recall_pos);
        recall_pos = save_recall_pos();

        past lval = LVal();
        if (lval) {
            advance();

            if (tok == tok_ASSIGN) {
                past exp = Exp();
                if (exp) {
                    advance();
                    if (tok == tok_SEMICOLON) {
                        return newOpcode("=", lval, exp);
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                past exp = Exp();
                if (exp) {
                    advance();

                    if (tok == tok_SEMICOLON) {
                        return exp;
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            }
        } else {
            recall(recall_pos);
            recall_pos = save_recall_pos();

            past exp = Exp();
            if (exp) {
                advance();

                if (tok == tok_SEMICOLON) {
                    return exp;
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                return Block();
            }
        }
    }
}

past Exp() {
    return AddExp();
}

past Cond() {
    return LOrExp();
}

past LVal() {
    int dimen = 0;
    past dimen_list = NULL;
    int recall_pos = save_recall_pos();

    advance();
    if (tok == tok_ID) {
        char id_str[100];
        strcpy(id_str, lex_str);

        while (1) {
            recall_pos = save_recall_pos();
            advance();

            if (tok == tok_LSQUARE) {
                past exp = Exp();

                if (exp) {
                    advance();

                    if (tok == tok_RSQUARE) {
                        dimen++;
                        AstAppendBro(&dimen_list, exp);
                        continue;
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);
                return newVar(id_str, dimen, dimen_list, NULL);
            }
        }
    } else
        return NULL;
}

past PrimaryExp() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_LPAR) {
        past exp = Exp();
        if (exp) {
            advance();

            if (tok == tok_RPAR) {
                return exp;
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        recall(recall_pos);

        past num = Number();
        if (num) {
            return num;
        } else {
            recall(recall_pos);

            return LVal();
        }
    }
}

past Number() {
    advance();

    if (tok == tok_INTEGER) {
        return newNum(atoi(lex_str));
    } else {
        return NULL;
    }
}

past UnaryExp() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_ID) {
        char id_str[100];
        strcpy(id_str, lex_str);

        advance();

        if (tok == tok_LPAR) {
            recall_pos = save_recall_pos();
            advance();

            if (tok == tok_RPAR) {
                return newFuncCall(id_str, NULL);
            } else {
                recall(recall_pos);

                past funcrparams = FuncRParams();
                if (funcrparams) {
                    advance();

                    if (tok == tok_RPAR) {
                        return newFuncCall(id_str, funcrparams);
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            }
        } else {
            recall(recall_pos);

            return PrimaryExp();
        }
    } else {
        recall(recall_pos);

        past primaryexp = PrimaryExp();
        if (primaryexp) {
            return primaryexp;
        } else {
            recall(recall_pos);

            past opcode = UnaryOp();
            if (opcode) {
                past unaryexp = UnaryExp();
                if (unaryexp) {
                    opcode->rs = unaryexp;

                    return opcode;
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        }
    }
}

past UnaryOp() {
    advance();
    if (tok == tok_ADD || tok == tok_SUB || tok == tok_NOT) {
        return newOpcode(lex_str, NULL, NULL);
    } else {
        return NULL;
    }
}

past FuncRParams() {
    past funcrparams_list = NULL;
    int recall_pos;

    past exp = Exp();
    if (exp) {
        recall_pos = save_recall_pos();
        advance();

        while (1) {
            if (tok == tok_COMMA) {
                past funcrparam = newFuncRParam(exp);
                AstAppendBro(&funcrparams_list, funcrparam);

                exp = Exp();
                if (exp) {
                    recall_pos = save_recall_pos();
                    advance();
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                past funcrparam = newFuncRParam(exp);
                AstAppendBro(&funcrparams_list, funcrparam);
                return funcrparams_list;
            }
        }
    } else {
        return NULL;
    }
}

past MulExp() {
    past unaryexp = UnaryExp();
    if (unaryexp) {
        int recall_pos = save_recall_pos();

        advance();
        if (tok == tok_SEMICOLON) {
            recall(recall_pos);

            return unaryexp;
        }

        recall(recall_pos);

        recall_pos = save_recall_pos();

        past mulexp_extend = MulExp_Extend();
        if (mulexp_extend) {
            mulexp_extend->ls = unaryexp;

            return mulexp_extend;
        } else {
            recall(recall_pos);

            return unaryexp;
        }
    } else {
        return NULL;
    }
}

past AddExp() {
    int recall_pos = save_recall_pos();

    past mulexp = MulExp();
    if (mulexp) {
        recall_pos = save_recall_pos();
        advance();

        if (tok == tok_SEMICOLON) {
            recall(recall_pos);

            return mulexp;
        }

        recall(recall_pos);
        int recall_pos = save_recall_pos();

        past addexp = AddExp_Extend();
        if (addexp) {
            addexp->ls = mulexp;

            return addexp;
        } else {
            recall(recall_pos);

            return mulexp;
        }
    } else {
        return NULL;
    }
}

past RelExp() {
    past addexp = AddExp();
    if (addexp) {
        int recall_pos = save_recall_pos();

        past relexp_extend = RelExp_Extend();
        if (relexp_extend) {
            relexp_extend->ls = addexp;
            return relexp_extend;
        } else {
            recall(recall_pos);
            return addexp;
        }
    } else
        return NULL;
}

past EqExp() {
    past relexp = RelExp();
    if (relexp) {
        int recall_pos = save_recall_pos();

        past eqexp_extend = EqExp_Extend();
        if (eqexp_extend) {
            eqexp_extend->ls = relexp;
        } else {
            recall(recall_pos);
            return relexp;
        }
    } else
        return NULL;
}

past LAndExp() {
    past eqexp = EqExp();
    if (eqexp) {
        int recall_pos = save_recall_pos();

        past landexp_extend = LAndExp_Extend();
        if (landexp_extend) {
            landexp_extend->ls = eqexp;
        } else {
            recall(recall_pos);
            return eqexp;
        }
    } else
        return NULL;
}

past LOrExp() {
    past landexp = LAndExp();
    if (landexp) {
        int recall_pos = save_recall_pos();

        past lorexp_extend = LOrExp_Extend();
        if (lorexp_extend) {
            lorexp_extend->ls = landexp;
        } else {
            recall(recall_pos);
            return landexp;
        }
    } else
        return NULL;
}

past MulExp_Extend(void) {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_MUL || tok == tok_DIV || tok == tok_MODULO) {
        char opcode_str[5];
        strcpy(opcode_str, lex_str);

        past unaryexp = UnaryExp();
        if (unaryexp) {
            recall_pos = save_recall_pos();
            past mulexp_extend = MulExp_Extend();

            if (mulexp_extend) {
                mulexp_extend->ls = unaryexp;
                past opcode = newOpcode(opcode_str, NULL, mulexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                past opcode = newOpcode(opcode_str, NULL, unaryexp);

                return opcode;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past AddExp_Extend(void) {
    int recall_pos = save_recall_pos();

    advance();
    if (tok == tok_ADD || tok == tok_SUB) {
        char opcode_str[5];
        strcpy(opcode_str, lex_str);

        past mulexp = MulExp();
        if (mulexp) {
            recall_pos = save_recall_pos();
            past addexp_extend = AddExp_Extend();

            if (addexp_extend) {
                addexp_extend->ls = mulexp;
                past opcode = newOpcode(opcode_str, NULL, addexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                return newOpcode(opcode_str, NULL, mulexp);
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past RelExp_Extend(void) {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_LESS || tok == tok_GREAT || tok == tok_LESSEQ ||
        tok == tok_GREATEQ) {
        char opcode_str[5];
        strcpy(opcode_str, lex_str);

        past addexp = AddExp();
        if (addexp) {
            recall_pos = save_recall_pos();
            past relexp_extend = RelExp_Extend();

            if (relexp_extend) {
                relexp_extend->ls = addexp;
                past opcode = newOpcode(opcode_str, NULL, relexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                past opcode = newOpcode(opcode_str, NULL, addexp);

                return opcode;
            }
        } else {
            return NULL;
        }
    } else {
        recall(recall_pos);

        return NULL;
    }
}

past EqExp_Extend(void) {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_EQ || tok == tok_NOTEQ) {
        char opcode_str[5];
        strcpy(opcode_str, lex_str);

        past relexp = RelExp();
        if (relexp) {
            recall_pos = save_recall_pos();
            past eqexp_extend = EqExp_Extend();

            if (eqexp_extend) {
                eqexp_extend->ls = relexp;
                past opcode = newOpcode(opcode_str, NULL, eqexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                past opcode = newOpcode(opcode_str, NULL, relexp);

                return opcode;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past LAndExp_Extend(void) {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_AND) {
        char opcode_str[5];
        strcpy(opcode_str, lex_str);

        past eqexp = EqExp();
        if (eqexp) {
            recall_pos = save_recall_pos();
            past landexp_extend = LAndExp_Extend();

            if (landexp_extend) {
                landexp_extend->ls = eqexp;
                past opcode = newOpcode(opcode_str, NULL, landexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                past opcode = newOpcode(opcode_str, NULL, eqexp);

                return opcode;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past LOrExp_Extend(void) {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_OR) {
        char opcode_str[5];
        strcpy(opcode_str, lex_str);

        past landexp = LAndExp();
        if (landexp) {
            recall_pos = save_recall_pos();
            past lorexp_extend = LOrExp_Extend();

            if (lorexp_extend) {
                lorexp_extend->ls = landexp;
                past opcode = newOpcode(opcode_str, NULL, lorexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                past opcode = newOpcode(opcode_str, NULL, landexp);

                return opcode;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

past ConstExp() {
    return AddExp();
}

past InitVal() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_LBRACKET) {
        past initval = InitVal();
        if (initval) {
            advance();

            while (1) {
                if (tok == tok_COMMA) {
                    past new_initval = InitVal();
                    if (new_initval) {
                        AstAppendBro(&initval, new_initval);
                        advance();
                    } else {
                        return NULL;
                    }
                } else
                    break;
            }

            if (tok == tok_RBRACKET) {
                return initval;
            } else {
                return NULL;
            }
        } else{
            if (tok == tok_RBRACKET)
                return initval;
            else
                return NULL;
        }
    } else {
        recall(recall_pos);
        return Exp();
    }
}

void ShowAst(past node, int nest) {
    if (!node)
        return;

    for (int i = 0; i < nest; i++)
        printf("	");

    if (node->nodeType == "Num")
        printf("%s %ld\n", node->nodeType, node->data.value);
    else if (node->nodeType == "expr")
        printf("%s '%c'\n", node->nodeType, (char)node->data.value);
    else if (node->nodeType == "VarDecl")
        printf("VarDecl: %s\n", node->data.type);
    else if (node->nodeType == "Varible"){
        if(node->data.dimen)
            printf("%s:  %d\n", node->data.id, node->data.dimen);
        else
            printf("%s:  %ld\n", node->data.id, node->data.value);
    }
    else if (node->nodeType == "ConstVar"){
        if(node->data.dimen)
            printf("%s:  %d\n", node->data.id, node->data.dimen);
        else
            printf("%s:  %ld\n", node->data.id, node->data.value);
    }
    else if (node->nodeType == "ConstDecl")
        printf("ConstDecl: %s\n", node->data.type);
    else if (node->nodeType == "FuncDef")
        printf("FuncDef: %s %s\n", node->data.id, node->data.type);
    else if (node->nodeType == "FuncFParam")
        printf("FuncFParam: %s %s %d\n", node->data.id, node->data.type, node->data.dimen);
    else if (node->nodeType == "FuncRParam")
        printf("FuncRParam: %s %s %d\n", node->data.id, node->data.type, node->data.dimen);
    else if (node->nodeType == "Block")
        printf("%s\n", node->nodeType);
    else if (node->nodeType == "Opcode")
        printf("%s\n", node->data.type);
    else if (node->nodeType == "IfStmt")
        printf("%s\n", node->nodeType);
    else if (node->nodeType == "ElseStmt")
        printf("%s\n", node->nodeType);
    else if (node->nodeType == "WhileStmt")
        printf("%s\n", node->nodeType);
    else if (node->nodeType == "ContinueStmt")
        printf("%s\n", node->nodeType);
    else if (node->nodeType == "FuncCall")
        printf("%s: %s\n", node->nodeType, node->data.id);
    else if (node->nodeType == "BreakStmt")
        printf("%s\n", node->nodeType);
    else if (node->nodeType == "ReturnStmt")
        printf("%s\n", node->nodeType);
    else;

    ShowAst(node->ls, nest + 1);
    ShowAst(node->rs, nest + 1);
    ShowAst(node->bro, nest);
}

void FreeAst(past node) {
    if (!node)
        return;

    FreeAst(node->ls);
    FreeAst(node->rs);
    FreeAst(node->bro);
    free(node);
}

// 递归下降分析
past Recursive_Parse(void) {
    first_get = 1;
    getChar();

    return CompUnit();
}

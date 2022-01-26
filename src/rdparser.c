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

past newAstNode() {
    past node = malloc(sizeof(ast));
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
    type->dataType = typename;
    return type;
}

past newNum(int value) {
    past num = newAstNode();
    num->nodeType = "IntValue";
    num->ivalue = value;
    return num;
}

// past newVariable_List(past variable, past variable_list){
// 	past newvariable_list = newAstNode();
// 	newvariable_list
// }

past newVar(char *name, int dimension, past dimension_list) {
    past var = newAstNode();
    strcpy(var->ident, name);
    var->nodeType = "Varible";
    var->ivalue = dimension;
    var->left = dimension_list;
    return var;
}

past newDimension_List(past dimension, past dimension_list) {
    past newdimension_list = newAstNode();
    newdimension_list->nodeType = "Dimension_List";
    newdimension_list->left = dimension;
    newdimension_list->right = dimension_list;
    return newdimension_list;
}

// past newDimension(int length){
// 	past dimension = newAstNode();
// 	dimension->nodeType = "Dimension";
// 	dimension->ivalue = length;
// 	return dimension;
// }

past newOpcode(char *op, past left, past right) {
    past opcode = newAstNode();
    opcode->nodeType = op;
    opcode->left = left;
    opcode->right = right;
    return opcode;
}

past newFuncDef_or_Decl_list(past funcdef_or_decl, past funcdef_or_decl_list) {
    past f_or_d_list = newAstNode();
    f_or_d_list->nodeType = "FuncDef_or_Decl_list";
    f_or_d_list->left = funcdef_or_decl;
    f_or_d_list->right = funcdef_or_decl_list;
    return f_or_d_list;
}

past newFuncDef(past funcfparams_list, past block, char *ident, char *rettype) {
    past funcdef = newAstNode();
    strcpy(funcdef->ident, ident);
    funcdef->nodeType = "FuncDef";
    funcdef->dataType = rettype;
    funcdef->left = funcfparams_list;
    funcdef->right = block;
    return funcdef;
}

past newFuncFParam(char *type, char *ident, int dimension,
					past dimension_list) {
    past funcfparam = newAstNode();
    funcfparam->nodeType = type;
    funcfparam->ivalue = dimension;
    funcfparam->left = dimension_list;
    strcpy(funcfparam->ident, ident);
    return funcfparam;
}

past newFuncFParams(past funcparam, past funcparams) {
    past newfuncparams = newAstNode();
    newfuncparams->left = funcparam;
    newfuncparams->right = funcparams;
    newfuncparams->nodeType = "FuncParams";
    return newfuncparams;
}

past newBlock_list(past decl_or_stmt, past block_list) {
    past newblock_list = newAstNode();
    newblock_list->nodeType = "Block_List";
    newblock_list->left = decl_or_stmt;
    newblock_list->right = block_list;
    return newblock_list;
}

past newBlock(past block_list) {
    past block = newAstNode();
    block->nodeType = "Block";
    block->right = block_list;
    return block;
}

past newInitVal(past exp_or_initval, past initvals) {
    past newinitval = newAstNode();
    newinitval->left = exp_or_initval;
    newinitval->right = initvals;

    if (!initvals)
        newinitval->nodeType = "InitVal";
    else
        newinitval->nodeType = "InitVals";

    return newinitval;
}

past newConstDef_List(past constdef, past constdef_list) {
    past newconstdef_list = newAstNode();
    newconstdef_list->nodeType = "ConstDefList";
    newconstdef_list->left = constdef;
    newconstdef_list->right = constdef_list;
    return constdef_list;
}

past newConstDecl(past type, past constdef_list) {
    past constdecl = newAstNode();
    constdecl->nodeType = "ConstDecl";
    constdecl->left = type;
    constdecl->right = constdef_list;
    return constdecl;
}

past newConstInitVal_List(past constinitval, past constinitval_list) {
    past newconstinitval_list = newAstNode();
    newconstinitval_list->nodeType = "ConstInitVal_List";
    newconstinitval_list->left = constinitval;
    newconstinitval_list->right = constinitval_list;
    return newconstinitval_list;
}

past newVarDecl(past type, past vardef_list) {
    past vardecl = newAstNode();
    vardecl->nodeType = "VarDecl";
    vardecl->left = type;
    vardecl->right = vardef_list;
    return vardecl;
}

past newVarDef_List(past vardef, past vardef_list) {
    past newvardef_list = newAstNode();
    newvardef_list->nodeType = "VarDef_List";
    newvardef_list->left = vardef;
    newvardef_list->right = vardef_list;
    return newvardef_list;
}

past newFunc(char *funcname, past funcrparams_list) {
    past func = newAstNode();
    strcpy(func->ident, funcname);
    func->nodeType = "Func";
    func->left = funcrparams_list;
    return func;
}

past newFuncRParams_List(past funcrparam, past funcrparams_list) {
    past newfuncrparams_list = newAstNode();
    newfuncrparams_list->nodeType = "FuncRParams_List";
    newfuncrparams_list->left = funcrparam;
    newfuncrparams_list->right = funcrparams_list;
    return newfuncrparams_list;
}

past newWhileStmt(past cond, past stmt) {
    past whilestmt = newAstNode();
    whilestmt->nodeType = "WhileStmt";
    whilestmt->left = cond;
    whilestmt->right = stmt;
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
    returnstmt->left = exp;
    return returnstmt;
}

past newBreakStmt() {
    past breakstmt = newAstNode();
    breakstmt->nodeType = "BreakStmt";
    return breakstmt;
}

past newSelectStmt(past ifstmt, past elsestmt) {
    past selectstmt = newAstNode();
    selectstmt->nodeType = "SelectStmt";
    selectstmt->left = ifstmt;
    selectstmt->right = elsestmt;
    return selectstmt;
}

past newIfStmt(past cond, past stmt) {
    past ifstmt = newAstNode();
    ifstmt->nodeType = "IfStmt";
    ifstmt->left = cond;
    ifstmt->right = stmt;
    return ifstmt;
}

past newElseStmt() {
    past elsestmt = newAstNode();
    elsestmt->nodeType = "ElseStmt";
    return elsestmt;
}

past newNullStmt() {
    past nullstmt = newAstNode();
    nullstmt->nodeType = "NullStmt";
    return nullstmt;
}

past CompUnit() {
    int recall_pos;
    past funcdef_or_decl_list = NULL;

    while (1) {
        recall_pos = save_recall_pos();
        advance();

        if (tok == tok_EOF) {
            return funcdef_or_decl_list;
        } else {
            recall(recall_pos);
            past funcdef = FuncDef();

            if (!funcdef) {
                recall(recall_pos);
                past decl = Decl();

                if (!decl) {
                    return NULL;
                } else {
                    funcdef_or_decl_list = newFuncDef_or_Decl_list(decl, funcdef_or_decl_list);
                }
            } else {
                funcdef_or_decl_list = newFuncDef_or_Decl_list(funcdef, funcdef_or_decl_list);
            }
        }
    }
}

past Decl() {
    int recall_pos = save_recall_pos(recall_pos);

    past constdecl = ConstDecl();
    if (constdecl != NULL) {
        return constdecl;
    } else {
        recall(recall_pos);

        past vardecl = VarDecl();
        if (vardecl != NULL) {
            return vardecl;
        } else {
            return NULL;
        }
    }
}

past ConstDecl() {
    past constdef_list = NULL;

    advance();
    if (tok == tok_CONST) {
        past btype = BType();
        if (btype != NULL) {
            past constdef = ConstDef();
            if (constdef != NULL) {
                constdef_list = newConstDef_List(constdef, constdef_list);

                advance();
                if (tok == tok_COMMA) {
                    while (1) {
                        past constdef = ConstDef();
                        if (constdef != NULL) {
                            constdef_list =
                                newConstDef_List(constdef, constdef_list);
                            advance();

                            if (tok != tok_COMMA)
                                break;
                        } else {
                            return NULL;
                        }
                    }

                    if (tok == tok_SEMICOLON) {
                        past constdecl = newConstDecl(btype, constdef_list);

                        return constdecl;
                    } else {
                        return NULL;
                    }
                } else if (tok == tok_SEMICOLON) {
                    past constdecl = newConstDecl(btype, constdef);

                    return constdecl;
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
    advance();

    if (tok == tok_ID) {
        char id_str[100];
        strcpy(id_str, lex_str);

        advance();

        if (tok == tok_ASSIGN) {
            past const_variable = newVar(id_str, 0, NULL);
            past constinitval = ConstInitVal();
            if (constinitval != NULL) {
                past constdef_assign =
                    newOpcode("=", const_variable, constinitval);

                return constdef_assign;
            } else {
                return NULL;
            }
        } else if (tok == tok_LSQUARE) {
            past constexp = ConstExp();
            if (constexp != NULL) {
                advance();

                if (tok == tok_RSQUARE) {
                    past dimension_list = newDimension_List(constexp, NULL);
                    past const_variable = newVar(id_str, 1, dimension_list);
                    advance();

                    if (tok == tok_ASSIGN) {
                        past constinitval = ConstInitVal();

                        if (constinitval != NULL) {
                            past constdef_assign =
                                newOpcode("=", const_variable, constdef_assign);

                            return constdef_assign;
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

    if (tok == tok_INT) {
        past type = newType(lex_str);
        return type;
    } else {
        return NULL;
    }
}

past VarDecl() {
    past vardef_list = NULL;
    int last_pos = save_recall_pos();

    past btype = BType();
    if (btype != NULL) {
        past vardef = VarDef();
        if (vardef != NULL) {
            advance();

            if (tok == tok_SEMICOLON) {
                past vardecl = newVarDecl(btype, vardef);

                return vardecl;
            } else {
                while (1) {
                    if (tok == tok_COMMA) {
                        vardef_list = newVarDef_List(vardef, vardef_list);

                        vardef = VarDef();
                        if (vardef != NULL)
                            advance();
                        else {
                            return NULL;
                        }
                    } else
                        break;
                }

                if (tok == tok_SEMICOLON) {
                    past vardecl = newVarDecl(btype, vardef);

                    return vardecl;
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
}

past VarDef() {
    int dimension = 0;
    past dimension_list = NULL;

    if (tok == tok_ID) {
        char id_str[100];
        strcpy(id_str, lex_str);

        int recall_pos = save_recall_pos();
        advance();

        if (tok == tok_ASSIGN) {
            past var = newVar(id_str, 0, NULL);
            past initval = InitVal();

            if (initval != NULL) {
                past opcode = newOpcode("=", var, initval);

                return opcode;
            } else {
                return NULL;
            }
        } else if (tok == tok_LSQUARE) {
            while (1) {
                if (tok == tok_LSQUARE) {
                    past constexp = ConstExp();
                    if (constexp != NULL) {
                        advance();

                        if (tok == tok_RSQUARE) {
                            dimension++;
                            dimension_list =
                                newDimension_List(constexp, dimension_list);
                            recall_pos = save_recall_pos();
                            advance();
                        } else {
                            return NULL;
                        }
                    } else {
                        return NULL;
                    }
                } else
                    break;
            }

            past var = newVar(id_str, dimension, dimension_list);

            if (tok == tok_ASSIGN) {
                past initval = InitVal();
                if (initval != NULL) {
                    past opcode = newOpcode("=", var, initval);

                    return opcode;
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                return var;
            }
        } else {
            recall(recall_pos);
            past var = newVar(id_str, 0, NULL);

            return var;
        }
    } else {
        return NULL;
    }
}

past FuncDef() {
    int last_pos;

    past type = FuncType();
    if (type != NULL) {
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
                    if (block != NULL) {
                        past funcdef =
                            newFuncDef(NULL, block, id_str, type->dataType);

                        return funcdef;
                    } else {
                        return NULL;
                    }
                } else {
                    recall(last_pos);

                    past funcfarams = FuncFParams();

                    if (funcfarams != NULL) {
                        advance();

                        if (tok == tok_RPAR) {
                            past block = Block();
                            if (block != NULL) {
                                past funcdef = newFuncDef(
                                    funcfarams, block, id_str, type->dataType);

                                return funcdef;
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
            if (constinitval != NULL) {
                advance();

                if (tok == tok_COMMA) {
                    constinitval_list =
                        newConstInitVal_List(constinitval, constinitval_list);
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

        past constexp = ConstExp();
        if (constexp != NULL) {
            return constexp;
        } else {
            return NULL;
        }
    }
}

past FuncType() {
    advance();

    if (tok == tok_VOID || tok == tok_INT) {
        past type = newType(lex_str);

        return type;
    } else {
        return NULL;
    }
}

past FuncFParams() {
    int recall_pos;

    past funcfparams = NULL;

    while (1) {
        past funcfparam = FuncFParam();

        if (funcfparam != NULL) {
            past funcfparams = newFuncFParams(funcfparam, funcfparams);

            recall_pos = save_recall_pos();
            advance();

            if (tok != tok_COMMA) {
                recall(recall_pos);

                return funcfparams;
            }
        } else {
            return NULL;
        }
    }
}

past FuncFParam() {
    int dimension = 0;
    past dimension_list = NULL;
    int recall_pos;

    if (BType() != NULL) {
        char type_str[100];
        strcpy(type_str, lex_str);

        advance();

        if (tok == tok_ID) {
            char id_str[100];
            strcpy(id_str, lex_str);

            recall_pos = save_recall_pos();
            advance();

            if (tok == tok_LSQUARE) {
                advance();

                if (tok == tok_RSQUARE) {
                    dimension++;
                    dimension_list = newDimension_List(NULL, NULL);

                    while (1) {
                        recall_pos = save_recall_pos();
                        advance();

                        if (tok == tok_LSQUARE) {
                            past exp = Exp();

                            if (exp != NULL) {
                                advance();

                                if (tok == tok_RSQUARE) {
                                    continue;
                                    dimension++;
                                    dimension_list =
                                        newDimension_List(exp, dimension_list);
                                } else {
                                    return NULL;
                                }
                            } else {
                                return NULL;
                            }
                        } else {
                            recall(recall_pos);
                            past funcfparam = newFuncFParam(
                                type_str, id_str, dimension, dimension_list);

                            return funcfparam;
                        }
                    }
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);
                past funcfparam = newFuncFParam(type_str, id_str, 0, NULL);

                return funcfparam;
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
    past block = NULL;
    past block_list = NULL;

    advance();

    if (tok == tok_LBRACKET) {
        recall_pos = save_recall_pos();

        advance();

        if (tok == tok_RBRACKET) {
            block = newBlock(NULL);

            return block;
        } else {
            recall(recall_pos);

            while (1) {
                past blockitem = BlockItem();
                if (blockitem != NULL) {
                    recall_pos = save_recall_pos();
                    advance();

                    if (tok == tok_RBRACKET) {
                        past block = newBlock(block_list);

                        return block;
                    } else {
                        block_list = newBlock_list(blockitem, block_list);
                        recall(recall_pos);
                    }
                } else {
                    return NULL;
                }
            }
        }
    } else {
        return NULL;
    }
}

past BlockItem() {
    int recall_pos = save_recall_pos();

    past stmt = Stmt();
    if (!stmt) {
        recall(recall_pos);

        past decl = Decl();
        if (decl != NULL) {
            return decl;
        } else {
            return NULL;
        }
    } else {
        return stmt;
    }
}

past Stmt() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_IF) {
        advance();

        if (tok == tok_LPAR) {
            past cond = Cond();
            if (cond != NULL) {
                advance();

                if (tok == tok_RPAR) {
                    past stmt = Stmt();
                    if (stmt != NULL) {
                        past ifstmt = newIfStmt(cond, stmt);

                        recall_pos = save_recall_pos();
                        advance();

                        if (tok == tok_ELSE) {
                            past else_stmt = Stmt();
                            if (else_stmt != NULL) {
                                past selectstmt =
                                    newSelectStmt(ifstmt, else_stmt);

                                return selectstmt;
                            } else {
                                return NULL;
                            }
                        } else {
                            recall(recall_pos);

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
            if (cond != NULL) {
                advance();
                if (tok == tok_RPAR) {
                    past stmt = Stmt();
                    if (stmt != NULL) {
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
            past breakstmt = newBreakStmt();

            return breakstmt;
        } else {
            return NULL;
        }
    } else if (tok == tok_CONTINUE) {
        advance();

        if (tok == tok_SEMICOLON) {
            past continuestmt = newContinueStmt();

            return continuestmt;
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
        if (exp != NULL) {
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
        if (lval != NULL) {
            advance();

            if (tok == tok_ASSIGN) {
                past exp = Exp();
                if (exp != NULL) {
                    advance();
                    if (tok == tok_SEMICOLON) {
                        past assign_stmt = newOpcode("=", lval, exp);

                        return assign_stmt;
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                past exp = Exp();
                if (exp != NULL) {
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

            past exp = Exp();
            if (exp != NULL) {
                advance();

                if (tok == tok_SEMICOLON) {
                    return exp;
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                past block = Block();
                if (block != NULL) {
                    return block;
                } else {
                    return NULL;
                }
            }
        }
    }
}

past Exp() {
    past exp = AddExp();

    if (exp != NULL) {
        return exp;
    } else {
        return NULL;
    }
}

past Cond() {
    past lorexp = LOrExp();
    if (lorexp != NULL) {
        return lorexp;
    } else {
        return NULL;
    }
}

past LVal() {
    past dimension_list = NULL;
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

                if (exp != NULL) {
                    advance();

                    if (tok == tok_RSQUARE) {
                        dimension_list = newDimension_List(exp, dimension_list);

                        continue;
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);

                past var = newVar(id_str, 0, dimension_list);

                return var;
            }
        }
    } else {
        return NULL;
    }
}

past PrimaryExp() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_LPAR) {
        past exp = Exp();
        if (exp != NULL) {
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
        if (num != NULL) {
            return num;
        } else {
            recall(recall_pos);

            past lval = LVal();
            if (lval != NULL) {
                return lval;
            } else {
                return NULL;
            }
        }
    }
}

past Number() {
    advance();

    if (tok == tok_INTEGER) {
        past num = newNum(atoi(lex_str));

        return num;
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
                past func = newFunc(id_str, NULL);

                return func;
            } else {
                recall(recall_pos);

                past funcrparams = FuncRParams();
                if (funcrparams != NULL) {
                    advance();

                    if (tok == tok_RPAR) {
                        past func = newFunc(id_str, funcrparams);

                        return func;
                    } else {
                        return NULL;
                    }
                } else {
                    return NULL;
                }
            }
        } else {
            recall(recall_pos);

            past primaryexp = PrimaryExp();
            if (primaryexp != NULL) {
                return primaryexp;
            } else {
                return NULL;
            }
        }
    } else {
        recall(recall_pos);

        past primaryexp = PrimaryExp();
        if (primaryexp != NULL) {
            return primaryexp;
        } else {
            recall(recall_pos);

            past opcode = UnaryOp();
            if (opcode != NULL) {
                past unaryexp = UnaryExp();
                if (unaryexp != NULL) {
                    opcode->right = unaryexp;

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
        past opcode = newOpcode(lex_str, NULL, NULL);

        return opcode;
    } else {
        return NULL;
    }
}

past FuncRParams() {
    past funcrparams_list = NULL;
    int recall_pos;

    past exp = Exp();
    if (exp != NULL) {
        recall_pos = save_recall_pos();
        advance();

        while (1) {
            if (tok == tok_COMMA) {
                funcrparams_list = newFuncRParams_List(exp, funcrparams_list);

                exp = Exp();
                if (exp != NULL) {
                    recall_pos = save_recall_pos();
                    advance();
                } else {
                    return NULL;
                }
            } else {
                recall(recall_pos);
                funcrparams_list = newFuncRParams_List(exp, funcrparams_list);

                return funcrparams_list;
            }
        }
    } else {
        return NULL;
    }
}

past MulExp() {
    past unaryexp = UnaryExp();
    if (unaryexp != NULL) {
        int recall_pos = save_recall_pos();

        advance();
        if (tok == tok_SEMICOLON) {
            recall(recall_pos);

            return unaryexp;
        }

        recall(recall_pos);

        recall_pos = save_recall_pos();

        past mulexp_extend = MulExp_Extend();
        if (mulexp_extend != NULL) {
            mulexp_extend->left = unaryexp;

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
    if (mulexp != NULL) {
        recall_pos = save_recall_pos();
        advance();

        if (tok == tok_SEMICOLON) {
            recall(recall_pos);

            return mulexp;
        }

        recall(recall_pos);

        int recall_pos = save_recall_pos();

        past addexp_extend = AddExp_Extend();
        if (addexp_extend != NULL) {
            addexp_extend->left = mulexp;

            return addexp_extend;
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
    if (addexp != NULL) {
        int recall_pos = save_recall_pos();

        past relexp_extend = RelExp_Extend();
        if (relexp_extend != NULL) {
            relexp_extend->left = addexp;
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
    if (relexp != NULL) {
        int recall_pos = save_recall_pos();

        past eqexp_extend = EqExp_Extend();
        if (eqexp_extend != NULL) {
            eqexp_extend->left = relexp;
        } else {
            recall(recall_pos);
            return relexp;
        }
    } else
        return NULL;
}

past LAndExp() {
    past eqexp = EqExp();
    if (eqexp != NULL) {
        int recall_pos = save_recall_pos();

        past landexp_extend = LAndExp_Extend();
        if (landexp_extend != NULL) {
            landexp_extend->left = eqexp;
        } else {
            recall(recall_pos);
            return eqexp;
        }
    } else
        return NULL;
}

past LOrExp() {
    past landexp = LAndExp();
    if (landexp != NULL) {
        int recall_pos = save_recall_pos();

        past lorexp_extend = LOrExp_Extend();
        if (lorexp_extend != NULL) {
            lorexp_extend->left = landexp;
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
        if (unaryexp != NULL) {
            recall_pos = save_recall_pos();
            past mulexp_extend = MulExp_Extend();

            if (mulexp_extend != NULL) {
                mulexp_extend->left = unaryexp;
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
        if (mulexp != NULL) {
            recall_pos = save_recall_pos();
            past addexp_extend = AddExp_Extend();

            if (addexp_extend != NULL) {
                addexp_extend->left = mulexp;
                past opcode = newOpcode(opcode_str, NULL, addexp_extend);

                return opcode;
            } else {
                recall(recall_pos);
                past opcode = newOpcode(opcode_str, NULL, mulexp);

                return opcode;
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
        if (addexp != NULL) {
            recall_pos = save_recall_pos();
            past relexp_extend = RelExp_Extend();

            if (relexp_extend != NULL) {
                relexp_extend->left = addexp;
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
        if (relexp != NULL) {
            recall_pos = save_recall_pos();
            past eqexp_extend = EqExp_Extend();

            if (eqexp_extend != NULL) {
                eqexp_extend->left = relexp;
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
        if (eqexp != NULL) {
            recall_pos = save_recall_pos();
            past landexp_extend = LAndExp_Extend();

            if (landexp_extend != NULL) {
                landexp_extend->left = eqexp;
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
        if (landexp != NULL) {
            recall_pos = save_recall_pos();
            past lorexp_extend = LOrExp_Extend();

            if (lorexp_extend != NULL) {
                lorexp_extend->left = landexp;
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
    past addexp = AddExp();
    if (addexp != NULL) {
        return addexp;
    } else {
        return NULL;
    }
}

past InitVal() {
    int recall_pos = save_recall_pos();
    advance();

    if (tok == tok_LBRACKET) {
        past initval = InitVal();
        if (initval != NULL) {
            past initvals = newInitVal(initval, initvals);
            advance();

            while (1) {
                if (tok == tok_COMMA) {
                    initval = InitVal();
                    if (initval != NULL) {
                        initvals = newInitVal(initval, initvals);
                        advance();
                    } else {
                        return NULL;
                    }
                } else
                    break;
            }

            if (tok == tok_RBRACKET) {
                return initvals;
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        recall(recall_pos);

        past exp = Exp();
        if (exp != NULL) {
            past initval = newInitVal(exp, NULL);

            return initval;
        } else {
            return NULL;
        }
    }
}

void ShowAst(past node, int nest) {
    if (!node)
        return;

    for (int i = 0; i < nest; i++)
        printf("  ");
    if (strcmp(node->nodeType, "intValue") == 0)
        printf("%s %d\n", node->nodeType, node->ivalue);
    else if (strcmp(node->nodeType, "expr") == 0)
        printf("%s '%c'\n", node->nodeType, (char)node->ivalue);
    // else if(strcmp)
    else
        ;
    ShowAst(node->left, nest + 1);
    ShowAst(node->right, nest + 1);
}

void FreeAst(past node) {
    if (!node)
        return;

    FreeAst(node->left);
    FreeAst(node->right);
    free(node);
}

// 递归下降分析
past Recursive_Parse(void) {
    past ret;

    first_get = 1;
    getChar();

    return ret = CompUnit();
}

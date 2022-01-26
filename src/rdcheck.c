#include "../inc/rdcheck.h"

int tok = 0;

void advance(){
	tok = lex_ana();
}

int save_recall_pos(){
	int recall_pos;

	if (last_get == 0)
		recall_pos = ftell(ana_file) - 2;
	else
		recall_pos = ftell(ana_file) - 1;

	if (recall_pos == -2)
		recall_pos = 0;

	// printf("recall_pos: %d\n", recall_pos);

	return recall_pos;
}

void recall(int last_pos){
	int idx2 = ftell(ana_file);
	int recall_len = idx2 - last_pos;

	fseek(ana_file, (-1 * recall_len), SEEK_CUR);
	first_get = 1;
	// printf("回溯新位置: %ld\n", ftell(ana_file));

	getChar();
}

bool CompUnit(){
	int recall_pos;

	while (1){
		recall_pos = save_recall_pos();
		advance();

		if (tok == tok_EOF)
			return true;
		else{
			recall(recall_pos);

			if (!FuncDef()){
				recall(recall_pos);

				if (!Decl())
					return false;
			}
		}
	}
}

bool Decl(){
	int recall_pos = save_recall_pos();
	if (ConstDecl())
		return true;
	else{
		recall(recall_pos);
		return VarDecl();
	}
}

bool ConstDecl(){
	advance();
	if (tok == tok_CONST){

		if (BType()){
			if (ConstDef()){
				advance();

				if (tok == tok_COMMA){
					while (1){
						if (ConstDef()){
							advance();

							if (tok != tok_COMMA)
								break;
						}
						else
							return false;
					}
					return tok == tok_SEMICOLON;
				}
				else return tok == tok_SEMICOLON;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool ConstDef(){
	advance();

	if (tok == tok_ID){
		advance();

		if (tok == tok_ASSIGN)
			return ConstInitVal();
		else if (tok == tok_LSQUARE){
			if (ConstExp()){
				advance();

				if (tok == tok_RSQUARE){
					advance();

					if (tok == tok_ASSIGN)
						return ConstInitVal();
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool BType(){
	advance();
	return tok == tok_INT;
}

bool VarDecl(){
	int last_pos = save_recall_pos();

	if (BType()){
		if (VarDef()){
			advance();

			if (tok == tok_SEMICOLON)
				return true;
			else{
				while (1){
					if (tok == tok_COMMA){

						if (VarDef())
							advance();
						else
							return false;
					}
					else
						break;
				}
				return tok == tok_SEMICOLON;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool VarDefSameLeftFactor(void){
	advance();
	return tok == tok_ID;
}

bool VarDef(){
	if (VarDefSameLeftFactor()){
		int recall_pos = save_recall_pos();
		advance();

		if (tok == tok_ASSIGN)
			return InitVal();
		else if (tok == tok_LSQUARE){
			while (1){
				if (tok == tok_LSQUARE){
					if (ConstExp()){
						advance();
						if (tok == tok_RSQUARE){
							recall_pos = save_recall_pos();
							advance();
						}
						else
							return false;
					}
					else
						return false;
				}
				else
					break;
			}

			if (tok == tok_ASSIGN)
				return InitVal();
			else{
				recall(recall_pos);
				return true;
			}
		}
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool FuncDef(){
	int last_pos;

	if (FuncType()){
		advance();

		if (tok == tok_ID){
			advance();

			if (tok == tok_LPAR){
				last_pos = save_recall_pos();
				advance();

				if (tok == tok_RPAR)
					return Block();
				else{
					recall(last_pos);

					if (FuncFParams()){
						advance();

						if (tok == tok_RPAR)
							return Block();
						else
							return false;
					}
					else
						return false;
				}
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool ConstInitVal(){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_LBRACKET){
		recall_pos = save_recall_pos();
		advance();

		if (tok == tok_RBRACKET)
			return true;

		recall(recall_pos);

		while (1){
			if (ConstInitVal()){
				advance();

				if (tok == tok_COMMA)
					continue;
				else if (tok == tok_RBRACKET)
					return true;
				else
					return false;
			}
			else
				return false;
		}
	}
	else{
		recall(recall_pos);
		return ConstExp();
	}
}

bool FuncType(){
	advance();
	return (tok == tok_VOID || tok == tok_INT);
}

bool FuncFParams(){
	int recall_pos;

	while (1){
		if (FuncFParam()){
			recall_pos = save_recall_pos();
			advance();

			if (tok != tok_COMMA){
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
}

bool FuncFParam(){
	int recall_pos;
	if (BType()){
		advance();

		if (tok == tok_ID){
			recall_pos = save_recall_pos();
			advance();

			if (tok == tok_LSQUARE){
				advance();

				if (tok == tok_RSQUARE){
					while (1){
						recall_pos = save_recall_pos();
						advance();

						if (tok == tok_LSQUARE){
							if (Exp()){
								advance();
								if (tok == tok_RSQUARE)
									continue;
								else
									return false;
							}
							else
								return false;
						}
						else{
							recall(recall_pos);
							return true;
						}
					}
				}
				else
					return false;
			}
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool Block(){
	int recall_pos;

	advance();

	if (tok == tok_LBRACKET){
		recall_pos = save_recall_pos();

		advance();

		if (tok == tok_RBRACKET)
			return true;
		else{
			recall(recall_pos);
			while (1){
				if (BlockItem()){
					recall_pos = save_recall_pos();
					advance();
					if (tok == tok_RBRACKET)
						return true;
					else
						recall(recall_pos);
				}
				else
					return false;
			}
		}
	}
	else
		return false;
}

bool BlockItem(){
	int recall_pos = save_recall_pos();
	if (!Stmt()){
		recall(recall_pos);
		return Decl();
	}
	else
		return true;
}

bool Stmt(){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_IF){
		advance();

		if (tok == tok_LPAR){
			if (Cond()){
				advance();
				if (tok == tok_RPAR){
					if (Stmt()){
						recall_pos = save_recall_pos();
						advance();

						if (tok == tok_ELSE)
							return Stmt();
						else{
							recall(recall_pos);
							return true;
						}
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else if (tok == tok_WHILE){
		advance();

		if (tok == tok_LPAR){
			if (Cond()){
				advance();

				if (tok == tok_RPAR)
					return Stmt();
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else if (tok == tok_BREAK){
		advance();
		return tok == tok_SEMICOLON;
	}
	else if (tok == tok_CONTINUE){
		advance();
		return tok == tok_SEMICOLON;
	}
	else if (tok == tok_RETURN){
		recall_pos = save_recall_pos();
		advance();

		if (tok == tok_SEMICOLON)
			return true;

		recall(recall_pos);

		if (Exp()){
			advance();
			return tok == tok_SEMICOLON;
		}
		else
			return false;
	}
	else if (tok == tok_SEMICOLON)
		return true;
	else{
		recall(recall_pos);
		recall_pos = save_recall_pos();

		if (LVal()){
			advance();
			if (tok == tok_ASSIGN){
				if (Exp()){
					advance();
					return tok == tok_SEMICOLON;
				}
				else
					return false;
			}
			else{
				recall(recall_pos);

				if (Exp()){
					advance();
					return tok == tok_SEMICOLON;
				}
				else
					return false;
			}
		}
		else{
			recall(recall_pos);
			if (Exp()){
				advance();
				return tok == tok_SEMICOLON;
			}
			else{
				recall(recall_pos);
				return Block();
			}
		}
	}
}

bool Exp(){
	return AddExp();
}

bool Cond(){
	return LOrExp();
}

bool LVal(){
	int recall_pos = save_recall_pos();

	advance();
	if (tok == tok_ID){
		while (1){
			recall_pos = save_recall_pos();
			advance();
			if (tok == tok_LSQUARE){
				if (Exp()){
					advance();
					if (tok == tok_RSQUARE)
						continue;
					else
						return false;
				}
				else
					return false;
			}
			else{
				recall(recall_pos);
				return true;
			}
		}
	}
	else
		return false;
}

bool PrimaryExp(){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_LPAR){
		if (Exp()){
			advance();
			return tok == tok_RPAR;
		}
		else
			return false;
	}
	else{
		recall(recall_pos);
		recall_pos = save_recall_pos();
		if (Number())
			return true;
		else{
			recall(recall_pos);
			return LVal();
		}
	}
}

bool Number(){
	advance();
	return tok == tok_INTEGER;
}

bool UnaryExp(){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_ID){

		advance();
		if (tok == tok_LPAR){

			recall_pos = save_recall_pos();
			advance();
			if (tok == tok_RPAR)
				return true;
			else{
				recall(recall_pos);
				if (FuncRParams()){
					advance();
					return tok == tok_RPAR;
				}
				else
					return false;
			}
		}
		else{
			recall(recall_pos);
			return PrimaryExp();
		}
	}
	else {
		recall(recall_pos);
		recall_pos = save_recall_pos();
		if (PrimaryExp())
			return true;
		else{
			recall(recall_pos);
			if (UnaryOp())
				return UnaryExp();
			else
				return false;
		}
	}
}

bool UnaryOp(){
	advance();
	return (tok == tok_ADD || tok == tok_SUB || tok == tok_NOT);
}

bool FuncRParams(){
	int recall_pos;

	if (Exp()){
		recall_pos = save_recall_pos();
		advance();

		while (1){
			if (tok == tok_COMMA){
				if (Exp()){
					recall_pos = save_recall_pos();
					advance();
				}
				else
					return false;
			}
			else{
				recall(recall_pos);
				return true;
			}
		}
	}
	else
		return false;
}

bool MulExp(){
	if (UnaryExp()){
		int recall_pos = save_recall_pos();

		advance();
		if (tok == tok_SEMICOLON){
			recall(recall_pos);
			return true;
		}

		recall(recall_pos);

		recall_pos = save_recall_pos();
		if (MulExp_Extend())
			return true;
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool AddExp(){
	int recall_pos = save_recall_pos();

	if (MulExp()){
		recall_pos = save_recall_pos();
		advance();

		if (tok == tok_SEMICOLON){
			recall(recall_pos);
			return true;
		}

		recall(recall_pos);

		int recall_pos = save_recall_pos();
		if (AddExp_Extend())
			return true;
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool RelExp(){
	if (AddExp()){
		int recall_pos = save_recall_pos();

		if (RelExp_Extend())
			return true;
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool EqExp(){
	if (RelExp()){
		int recall_pos = save_recall_pos();

		if (EqExp_Extend())
			return true;
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool LAndExp(){
	if (EqExp()){
		int recall_pos = save_recall_pos();

		if (LAndExp_Extend())
			return true;
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool LOrExp(){
	if (LAndExp()){
		int recall_pos = save_recall_pos();

		if (LOrExp_Extend()){
			return true;
		}
		else{
			recall(recall_pos);
			return true;
		}
	}
	else
		return false;
}

bool MulExp_Extend(void){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_MUL || tok == tok_DIV || tok == tok_MODULO){
		if (UnaryExp()){
			recall_pos = save_recall_pos();
			if (MulExp_Extend())
				return true;
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool AddExp_Extend(void){
	int recall_pos = save_recall_pos();

	advance();
	if (tok == tok_ADD || tok == tok_SUB){
		if (MulExp()){
			recall_pos = save_recall_pos();
			if (AddExp_Extend())
				return true;
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool RelExp_Extend(void){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_LESS || tok == tok_GREAT || tok == tok_LESSEQ || tok == tok_GREATEQ){
		if (AddExp()){
			recall_pos = save_recall_pos();
			if (RelExp_Extend())
				return true;
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool EqExp_Extend(void){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_EQ || tok == tok_NOTEQ){
		if (RelExp()){
			recall_pos = save_recall_pos();
			if (EqExp_Extend())
				return true;
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool LAndExp_Extend(void){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_AND){
		if (EqExp()){
			recall_pos = save_recall_pos();
			if (LAndExp_Extend())
				return true;
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool LOrExp_Extend(void){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_OR){
		if (LAndExp()){
			recall_pos = save_recall_pos();
			if (LOrExp_Extend())
				return true;
			else{
				recall(recall_pos);
				return true;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool ConstExp(){
	return AddExp();
}

bool InitVal(){
	int recall_pos = save_recall_pos();
	advance();

	if (tok == tok_LBRACKET){
		if (InitVal()){
			advance();

			while (1){
				if (tok == tok_COMMA){

					if (InitVal())
						advance();
					else
						return false;
				}
				else
					break;
			}
			return tok == tok_RBRACKET;
		}
		else
			return false;
	}
	else{
		recall(recall_pos);
		return Exp();
	}
}

// 递归下降分析
bool Recursive_Parse(void){
	bool ret;

	first_get = 1;
	getChar();

	return ret = CompUnit();
}

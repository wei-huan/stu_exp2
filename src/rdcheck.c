#include "rdcheck.h"

int tok = 0;

void advance(){
	tok = lex_ana();
}

int save_recall_pos(){
	int recall_pos;

	if(last_get == 0)
		recall_pos = ftell(ana_file) - 2;
	else
		recall_pos = ftell(ana_file) - 1;

	if(recall_pos == -2)
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

	while(1){
		recall_pos = save_recall_pos();
		advance();

		if(tok == tok_EOF){
#ifdef DEBUG_MODE
			printf("CompUnit out1\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else {
			recall(recall_pos);

			if(!FuncDef()){
				recall(recall_pos);

				if(!Decl()){
#ifdef DEBUG_MODE
					printf("CompUnit out2\n");
#endif /*DEBUG_MODE*/
					return false;
				}
				else{
#ifdef DEBUG_MODE
					printf("成功, 完成一轮CompUnit分析\n");
#endif /*DEBUG_MODE*/
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("成功, 完成一轮CompUnit分析\n");
#endif /*DEBUG_MODE*/
			}
		}
	}
}

bool Decl(){

#ifdef DEBUG_MODE
	printf("Decl in\n");
#endif /*DEBUG_MODE*/
	int recall_pos = save_recall_pos(recall_pos);

	if(ConstDecl()){
#ifdef DEBUG_MODE
		printf("Decl out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
		recall(recall_pos);

		if(VarDecl()){
#ifdef DEBUG_MODE
			printf("Decl out2 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
#ifdef DEBUG_MODE
			printf("Decl out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
}

bool ConstDecl(){

#ifdef DEBUG_MODE
	printf("ConstDecl in\n");
#endif /*DEBUG_MODE*/

	advance();
	if(tok == tok_CONST){

		if(BType()){

			if(ConstDef()){
				advance();

				if(tok == tok_COMMA){
					while (1){
						if(ConstDef()){
							advance();

							if(tok != tok_COMMA)
								break;
						}
						else{
#ifdef DEBUG_MODE
							printf("ConstDecl out1 false\n");
#endif /*DEBUG_MODE*/
							return false;
						}
					}

					if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
						printf("ConstDecl out2 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else{
#ifdef DEBUG_MODE
						printf("ConstDecl out3 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}

				}
				else if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
					printf("ConstDecl out4 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("ConstDecl out5 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("ConstDecl out6 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}

		}
		else{
#ifdef DEBUG_MODE
			printf("ConstDecl out7 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("ConstDecl out8 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool ConstDef(){
#ifdef DEBUG_MODE
	printf("ConstDef in\n");
#endif /*DEBUG_MODE*/
	advance();

	if(tok == tok_ID){
		advance();

		if(tok == tok_ASSIGN){
			if(ConstInitVal()){
#ifdef DEBUG_MODE
				printf("ConstDef out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("ConstDef out2 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else if(tok == tok_LSQUARE){

			if(ConstExp()){
				advance();

				if(tok == tok_RSQUARE){
					advance();

					if(tok == tok_ASSIGN){

						if(ConstInitVal()){
#ifdef DEBUG_MODE
							printf("ConstDef out3 true\n");
#endif /*DEBUG_MODE*/
							return true;
						}
						else{
#ifdef DEBUG_MODE
							printf("ConstDef out4 false\n");
#endif /*DEBUG_MODE*/
							return false;
						}
					}
					else{
#ifdef DEBUG_MODE
						printf("ConstDef out5 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("ConstDef out6 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("ConstDef out7 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("ConstDef out8 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("ConstDef out9 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool BType(){
#ifdef DEBUG_MODE
	printf("BType in\n");
#endif /*DEBUG_MODE*/
	advance();

	if(tok == tok_INT){
#ifdef DEBUG_MODE
		printf("BType out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("BType out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool VarDecl(){
#ifdef DEBUG_MODE
	printf("VarDecl in\n");
#endif /*DEBUG_MODE*/
	int last_pos = save_recall_pos();

	if(BType()){

		if(VarDef()){

			advance();

			if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
				printf("VarDecl out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				while(1){
					if(tok == tok_COMMA){

						if(VarDef())
							advance();
						else{
#ifdef DEBUG_MODE
							printf("VarDecl out2 false\n");
#endif /*DEBUG_MODE*/
							return false;
						}
					}
					else
						break;
				}

				if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
					printf("VarDecl out4 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("VarDecl out5 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}

			}
		}
		else{
#ifdef DEBUG_MODE
			printf("VarDecl out6 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("VarDecl out7 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool VarDefSameLeftFactor(void){
#ifdef DEBUG_MODE
	printf("VarDefSameLeftFactor in\n");
#endif /*DEBUG_MODE*/

	advance();

	if(tok == tok_ID){
#ifdef DEBUG_MODE
		printf("VarDefSameLeftFactor out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("VarDefSameLeftFactor out1 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool VarDef(){
#ifdef DEBUG_MODE
	printf("VarDef in\n");
#endif /*DEBUG_MODE*/

	if(VarDefSameLeftFactor()){
		int recall_pos = save_recall_pos();
		advance();

		if(tok == tok_ASSIGN){
			if(InitVal()){
#ifdef DEBUG_MODE
				printf("VarDef out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("VarDef out2 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else if(tok == tok_LSQUARE){
			while(1){
				if(tok == tok_LSQUARE){
					if(ConstExp()){

						advance();

						if(tok == tok_RSQUARE){
							recall_pos = save_recall_pos();
							advance();
						}
						else{
#ifdef DEBUG_MODE
							printf("VarDef out3 false\n");
#endif /*DEBUG_MODE*/
							return false;
						}
					}
					else{
#ifdef DEBUG_MODE
						printf("VarDef out4 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else
					break;
			}

			if(tok == tok_ASSIGN){

				if(InitVal()){
#ifdef DEBUG_MODE
					printf("VarDef out5 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("VarDef out6 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("VarDef out7 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
			recall(recall_pos);
#ifdef DEBUG_MODE
			printf("VarDef out8 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("VarDef out9 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}


bool FuncDef(){
#ifdef DEBUG_MODE
	printf("FuncDef in\n");
#endif /*DEBUG_MODE*/
	int last_pos;

	if(FuncType()){
		advance();

		if(tok == tok_ID){
			advance();

			if(tok == tok_LPAR){
				last_pos = save_recall_pos();
				advance();

				if(tok == tok_RPAR){
					if(Block()){
#ifdef DEBUG_MODE
						printf("FuncDef out1 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else{
#ifdef DEBUG_MODE
						printf("FuncDef out2 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
					recall(last_pos);

					if(FuncFParams()){
						advance();

						if(tok == tok_RPAR){
							if(Block()){
#ifdef DEBUG_MODE
								printf("FuncDef out3 true\n");
#endif /*DEBUG_MODE*/
								return true;
							}
							else{
#ifdef DEBUG_MODE
								printf("FuncDef out4 false\n");
#endif /*DEBUG_MODE*/
								return false;
							}
						}
						else{
#ifdef DEBUG_MODE
							printf("FuncDef out5 false\n");
#endif /*DEBUG_MODE*/
							return false;
						}
					}
					else{
#ifdef DEBUG_MODE
						printf("FuncDef out6 true\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}

			}
			else{
#ifdef DEBUG_MODE
				printf("FuncDef out7 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("FuncDef out8 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("FuncDef out9 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool ConstInitVal(){
#ifdef DEBUG_MODE
	printf("ConstInitVal in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_LBRACKET){
		recall_pos = save_recall_pos();
		advance();

		if(tok == tok_RBRACKET){
#ifdef DEBUG_MODE
			printf("ConstInitVal out1 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}

		recall(recall_pos);

		while(1){
			if(ConstInitVal()){
				advance();

				if(tok == tok_COMMA)
					continue;
				else if(tok == tok_RBRACKET){
#ifdef DEBUG_MODE
					printf("ConstInitVal out1 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("ConstInitVal out2 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("ConstInitVal out3 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
	}
	else{
		recall(recall_pos);

		if(ConstExp()){
#ifdef DEBUG_MODE
			printf("ConstInitVal out4 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
#ifdef DEBUG_MODE
			printf("ConstInitVal out5 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
}

bool FuncType(){
#ifdef DEBUG_MODE
	printf("FuncType in\n");
#endif /*DEBUG_MODE*/
	advance();

	if(tok == tok_VOID || tok == tok_INT){
#ifdef DEBUG_MODE
		printf("FuncType out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("FuncType out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool FuncFParams(){
#ifdef DEBUG_MODE
	printf("FuncFParams in\n");
#endif /*DEBUG_MODE*/

	int recall_pos;

	while(1){
		if(FuncFParam()){
			recall_pos = save_recall_pos();
			advance();

			if(tok != tok_COMMA){
				recall(recall_pos);

#ifdef DEBUG_MODE
				printf("FuncFParams out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{

#ifdef DEBUG_MODE
			printf("FuncFParams out2 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
}

bool FuncFParam(){
#ifdef DEBUG_MODE
	printf("FuncFParam in\n");
#endif /*DEBUG_MODE*/

	int recall_pos;

	if(BType()){
		advance();

		if(tok == tok_ID){
			recall_pos = save_recall_pos();
			advance();

			if(tok == tok_LSQUARE){
				advance();

				if(tok == tok_RSQUARE){
					while(1){
						recall_pos = save_recall_pos();
						advance();

						if(tok == tok_LSQUARE){
							if(Exp()){
								advance();

								if(tok == tok_RSQUARE){
									continue;
								}
								else{

#ifdef DEBUG_MODE
									printf("FuncFParam out1 false\n");
#endif /*DEBUG_MODE*/
									return false;
								}
							}
							else{
#ifdef DEBUG_MODE
								printf("FuncFParam out2 false\n");
#endif /*DEBUG_MODE*/
								return false;
							}
						}
						else{
							recall(recall_pos);
#ifdef DEBUG_MODE
							printf("FuncFParam out3 true\n");
#endif /*DEBUG_MODE*/
							return true;
						}
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("FuncFParam out4 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
				recall(recall_pos);

#ifdef DEBUG_MODE
				printf("FuncFParam out5 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("FuncFParam out6 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("FuncFParam out7 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool Block(){
#ifdef DEBUG_MODE
	printf("Block in\n");
#endif /*DEBUG_MODE*/
	int recall_pos;

	advance();

	if(tok == tok_LBRACKET){
		recall_pos = save_recall_pos();

		advance();

		if(tok == tok_RBRACKET){
#ifdef DEBUG_MODE
			printf("Block out1 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
			recall(recall_pos);

			while(1){
				if(BlockItem()){
					recall_pos = save_recall_pos();
					advance();

					if(tok == tok_RBRACKET){
#ifdef DEBUG_MODE
						printf("Block out2 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else
						recall(recall_pos);
				}
				else{
#ifdef DEBUG_MODE
					printf("Block out3 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}

			}
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("Block out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool BlockItem(){
#ifdef DEBUG_MODE
	printf("BlockItem in\n");
#endif /*DEBUG_MODE*/
	int recall_pos = save_recall_pos();

	if(!Stmt()){
		recall(recall_pos);

		if(Decl()){
#ifdef DEBUG_MODE
			printf("BlockItem out1 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
#ifdef DEBUG_MODE
			printf("BlockItem out2 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("BlockItem out3 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
}

bool Stmt(){
#ifdef DEBUG_MODE
	printf("Stmt in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_IF){
		advance();

		if(tok == tok_LPAR){

			if(Cond()){
				advance();

				if(tok == tok_RPAR){

					if(Stmt()){
						recall_pos = save_recall_pos();
						advance();

						if(tok == tok_ELSE){

							if(Stmt()){
#ifdef DEBUG_MODE
								printf("Stmt out1 true\n");
#endif /*DEBUG_MODE*/
								return true;
							}
							else{
#ifdef DEBUG_MODE
								printf("Stmt out2 false\n");
#endif /*DEBUG_MODE*/
								return false;
							}
						}
						else{
							recall(recall_pos);
#ifdef DEBUG_MODE
							printf("Stmt out3 true\n");
#endif /*DEBUG_MODE*/
							return true;
						}
					}
					else{
#ifdef DEBUG_MODE
						printf("Stmt out4 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("Stmt out5 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("Stmt out6 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("Stmt out7 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else if(tok == tok_WHILE){
		advance();

		if(tok == tok_LPAR){
			if(Cond()){
				advance();

				if(tok == tok_RPAR){

					if(Stmt()){
#ifdef DEBUG_MODE
						printf("Stmt out8 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else{
#ifdef DEBUG_MODE
						printf("Stmt out9 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("Stmt out10 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("Stmt out11 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("Stmt out12 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else if(tok == tok_BREAK){
		advance();

		if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
			printf("Stmt out13 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
#ifdef DEBUG_MODE
			printf("Stmt out14 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else if(tok == tok_CONTINUE){
		advance();

		if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
			printf("Stmt out15 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
#ifdef DEBUG_MODE
			printf("Stmt out16 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}

	}
	else if(tok == tok_RETURN){
		recall_pos = save_recall_pos();
		advance();

		if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
			printf("Stmt out17 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}

		recall(recall_pos);

		if(Exp()){
			advance();

			if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
				printf("Stmt out17 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("Stmt out18 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("Stmt out19 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
		printf("Stmt out20 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
		recall(recall_pos);
		recall_pos = save_recall_pos();

		if(LVal()){
			advance();

			if(tok == tok_ASSIGN){
				if(Exp()){

					advance();
					if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
						printf("Stmt out21 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else{
#ifdef DEBUG_MODE
						printf("Stmt out22 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("Stmt out23 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
				recall(recall_pos);

				if(Exp()){
					advance();

					if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
						printf("Stmt out24 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else{
#ifdef DEBUG_MODE
						printf("Stmt out25 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("Stmt out26 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
		}
		else{
			recall(recall_pos);

			if(Exp()){
				advance();

				if(tok == tok_SEMICOLON){
#ifdef DEBUG_MODE
					printf("Stmt out27 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("Stmt out28 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else {
				recall(recall_pos);

				if(Block()){
#ifdef DEBUG_MODE
					printf("Stmt out29 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("Stmt out30 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
		}
	}
}

bool Exp(){
#ifdef DEBUG_MODE
	printf("Exp in\n");
#endif /*DEBUG_MODE*/

	if(AddExp()){
#ifdef DEBUG_MODE
		printf("Exp out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("Exp out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool Cond(){
#ifdef DEBUG_MODE
	printf("Cond in\n");
#endif /*DEBUG_MODE*/

	if(LOrExp()){
#ifdef DEBUG_MODE
		printf("Cond out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("Cond out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool LVal(){
#ifdef DEBUG_MODE
	printf("LVal in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();

	advance();
	if(tok == tok_ID){

		while (1){
			recall_pos = save_recall_pos();
			advance();

			if(tok == tok_LSQUARE){

				if(Exp()){
					advance();

					if(tok == tok_RSQUARE){
						continue;
					}
					else{
#ifdef DEBUG_MODE
						printf("LVal out1 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("LVal out2 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("LVal out3 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("LVal out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool PrimaryExp(){

#ifdef DEBUG_MODE
	printf("PrimaryExp in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_LPAR){
		if(Exp()){
			advance();

			if(tok == tok_RPAR){
#ifdef DEBUG_MODE
				printf("PrimaryExp out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("PrimaryExp out2 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("PrimaryExp out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
		recall(recall_pos);

		if(Number()){
#ifdef DEBUG_MODE
			printf("PrimaryExp out4 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
			recall(recall_pos);

			if(LVal()){
#ifdef DEBUG_MODE
				printf("PrimaryExp out5 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("PrimaryExp out6 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
	}
}

bool Number(){
#ifdef DEBUG_MODE
	printf("Number in\n");
#endif /*DEBUG_MODE*/

	advance();

	if(tok == tok_INTEGER){
#ifdef DEBUG_MODE
		printf("Number out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("Number out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool UnaryExp(){
#ifdef DEBUG_MODE
	printf("UnaryExp in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_ID){
		advance();

		if(tok == tok_LPAR){
			recall_pos = save_recall_pos();
			advance();

			if(tok == tok_RPAR){
#ifdef DEBUG_MODE
				printf("UnaryExp out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				recall(recall_pos);

				if(FuncRParams()){
					advance();

					if(tok == tok_RPAR){
#ifdef DEBUG_MODE
						printf("UnaryExp out2 true\n");
#endif /*DEBUG_MODE*/
						return true;
					}
					else{
#ifdef DEBUG_MODE
						printf("UnaryExp out3 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else{
#ifdef DEBUG_MODE
					printf("UnaryExp out4 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
		}
		else{
			recall(recall_pos);

			if(PrimaryExp()){
#ifdef DEBUG_MODE
				printf("UnaryExp out5 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("UnaryExp out6 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
	}
	else {
		recall(recall_pos);

		if(PrimaryExp()){
#ifdef DEBUG_MODE
			printf("UnaryExp out7 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else {
			recall(recall_pos);

			if(UnaryOp()){
				if(UnaryExp()){
#ifdef DEBUG_MODE
					printf("UnaryExp out8 true\n");
#endif /*DEBUG_MODE*/
					return true;
				}
				else{
#ifdef DEBUG_MODE
					printf("UnaryExp out9 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
#ifdef DEBUG_MODE
				printf("UnaryExp out10 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
	}
}

bool UnaryOp(){
#ifdef DEBUG_MODE
	printf("UnaryOp in\n");
#endif /*DEBUG_MODE*/

	advance();
	if(tok == tok_ADD || tok == tok_SUB || tok == tok_NOT){
#ifdef DEBUG_MODE
		printf("UnaryOp out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("UnaryOp out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool FuncRParams(){
#ifdef DEBUG_MODE
	printf("FuncRParams in\n");
#endif /*DEBUG_MODE*/

	int recall_pos;

	if(Exp()){
		recall_pos = save_recall_pos();
		advance();

		while (1){
			if(tok == tok_COMMA){
				if(Exp()){
					recall_pos = save_recall_pos();
					advance();
				}
				else{
#ifdef DEBUG_MODE
					printf("FuncRParams out1 false\n");
#endif /*DEBUG_MODE*/
					return false;
				}
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("FuncRParams out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("FuncRParams out3 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool MulExp(){
#ifdef DEBUG_MODE
	printf("MulExp in\n");
#endif /*DEBUG_MODE*/

	if(UnaryExp()){
		int recall_pos = save_recall_pos();

		advance();
		if(tok == tok_SEMICOLON){
			recall(recall_pos);
#ifdef DEBUG_MODE
			printf("MulExp out1 true\n");
#endif /*DEBUG_MODE*/

			return true;
		}

		recall(recall_pos);

		recall_pos = save_recall_pos();
		if(MulExp_Extend()){
#ifdef DEBUG_MODE
			printf("MulExp out2 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
			recall(recall_pos);
#ifdef DEBUG_MODE
			printf("MulExp out3 false\n");
#endif /*DEBUG_MODE*/
			return true;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("MulExp out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool AddExp(){
#ifdef DEBUG_MODE
	printf("AddExp in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();

	if(MulExp()){
		recall_pos = save_recall_pos();
		advance();

		if(tok == tok_SEMICOLON){
			recall(recall_pos);
#ifdef DEBUG_MODE
			printf("AddExp out1 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}

		recall(recall_pos);

		int recall_pos = save_recall_pos();
		if(AddExp_Extend()){
#ifdef DEBUG_MODE
			printf("AddExp out2 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
			recall(recall_pos);
#ifdef DEBUG_MODE
			printf("AddExp out3 false\n");
#endif /*DEBUG_MODE*/
			return true;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("AddExp out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool RelExp(){
#ifdef DEBUG_MODE
	printf("RelExp in\n");
#endif /*DEBUG_MODE*/

	if(AddExp()){
		int recall_pos = save_recall_pos();

		if(RelExp_Extend()){
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

bool EqExp(){
#ifdef DEBUG_MODE
	printf("EqExp in\n");
#endif /*DEBUG_MODE*/

	if(RelExp()){
		int recall_pos = save_recall_pos();

		if(EqExp_Extend()){
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

bool LAndExp(){
#ifdef DEBUG_MODE
	printf("LAndExp in\n");
#endif /*DEBUG_MODE*/

	if(EqExp()){
		int recall_pos = save_recall_pos();

		if(LAndExp_Extend()){
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

bool LOrExp(){
#ifdef DEBUG_MODE
	printf("LOrExp in\n");
#endif /*DEBUG_MODE*/

	if(LAndExp()){

		int recall_pos = save_recall_pos();

		if(LOrExp_Extend()){
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
#ifdef DEBUG_MODE
	printf("MulExp_Extend in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_MUL || tok == tok_DIV || tok == tok_MODULO){
		if(UnaryExp()){

			recall_pos = save_recall_pos();
			if(MulExp_Extend()){
#ifdef DEBUG_MODE
				printf("MulExp_Extend out1 true\n");
#endif /*DEBUG_MODE*/

				return true;
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("MulExp_Extend out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("MulExp_Extend out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("MulExp_Extend out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool AddExp_Extend(void){
#ifdef DEBUG_MODE
	printf("AddExp_Extend in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();

	advance();
	if(tok == tok_ADD || tok == tok_SUB){
		if(MulExp()){

			recall_pos = save_recall_pos();
			if(AddExp_Extend()){
#ifdef DEBUG_MODE
				printf("AddExp_Extend out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("AddExp_Extend out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("AddExp_Extend out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("AddExp_Extend out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool RelExp_Extend(void){
#ifdef DEBUG_MODE
	printf("RelExp_Extend in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_LESS || tok == tok_GREAT || tok == tok_LESSEQ || tok == tok_GREATEQ){

		if(AddExp()){

			recall_pos = save_recall_pos();
			if(RelExp_Extend()){
#ifdef DEBUG_MODE
				printf("RelExp_Extend out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("RelExp_Extend out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("RelExp_Extend out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("RelExp_Extend out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool EqExp_Extend(void){
#ifdef DEBUG_MODE
	printf("EqExp_Extend in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_EQ || tok == tok_NOTEQ){
		if(RelExp()){

			recall_pos = save_recall_pos();
			if(EqExp_Extend()){
#ifdef DEBUG_MODE
				printf("EqExp_Extend out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("EqExp_Extend out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("EqExp_Extend out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("EqExp_Extend out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool LAndExp_Extend(void){
#ifdef DEBUG_MODE
	printf("LAndExp_Extend in\n");
#endif /*DEBUG_MODE*/
	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_AND){
		if(EqExp()){

			recall_pos = save_recall_pos();
			if(LAndExp_Extend()){
#ifdef DEBUG_MODE
				printf("LAndExp_Extend out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("LAndExp_Extend out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("LAndExp_Extend out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("LAndExp_Extend out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool LOrExp_Extend(void){
#ifdef DEBUG_MODE
	printf("LOrExp_Extend in\n");
#endif /*DEBUG_MODE*/
	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_OR){
		if(LAndExp()){

			recall_pos = save_recall_pos();
			if(LOrExp_Extend()){
#ifdef DEBUG_MODE
				printf("LOrExp_Extend out1 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
				recall(recall_pos);
#ifdef DEBUG_MODE
				printf("LOrExp_Extend out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("LOrExp_Extend out3 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
#ifdef DEBUG_MODE
		printf("LOrExp_Extend out4 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool ConstExp(){
#ifdef DEBUG_MODE
	printf("ConstExp in\n");
#endif /*DEBUG_MODE*/

	if(AddExp()){
#ifdef DEBUG_MODE
		printf("ConstExp out1 true\n");
#endif /*DEBUG_MODE*/
		return true;
	}
	else{
#ifdef DEBUG_MODE
		printf("ConstExp out2 false\n");
#endif /*DEBUG_MODE*/
		return false;
	}
}

bool InitVal(){
#ifdef DEBUG_MODE
	printf("InitVal in\n");
#endif /*DEBUG_MODE*/

	int recall_pos = save_recall_pos();
	advance();

	if(tok == tok_LBRACKET){

		if(InitVal()){
			advance();

			while(1){
				if(tok == tok_COMMA){

					if(InitVal())
						advance();
					else{
#ifdef DEBUG_MODE
						printf("InitVal out1 false\n");
#endif /*DEBUG_MODE*/
						return false;
					}
				}
				else
					break;
			}

			if(tok == tok_RBRACKET){
#ifdef DEBUG_MODE
				printf("InitVal out2 true\n");
#endif /*DEBUG_MODE*/
				return true;
			}
			else{
#ifdef DEBUG_MODE
				printf("InitVal out3 false\n");
#endif /*DEBUG_MODE*/
				return false;
			}
		}
		else{
#ifdef DEBUG_MODE
			printf("InitVal out4 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
	else{
		recall(recall_pos);

		if(Exp()){
#ifdef DEBUG_MODE
			printf("InitVal out5 true\n");
#endif /*DEBUG_MODE*/
			return true;
		}
		else{
#ifdef DEBUG_MODE
			printf("InitVal out6 false\n");
#endif /*DEBUG_MODE*/
			return false;
		}
	}
}

// 递归下降分析
bool Recursive_Parse(void){
	bool ret;

	first_get = 1;
	getChar();

	return ret = CompUnit();
}

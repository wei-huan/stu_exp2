TAR0 := man_lex
TAR1 := check
TAR2 := parser

# 参数
ALL ?= 0

# 编译器
CC := gcc
AR := ar

# 路径
INC := inc
SRC := src
LIB := lib
TAR := target
TEST := test/test_cases

$(TAR0):
	$(CC) -c $(SRC)/$(TAR0).c -o $(TAR)/$(TAR0).o -I $(INC)

lib$(TAR0): $(TAR0)
	$(AR) cqs $(LIB)/lib$(TAR0).a $(TAR)/$(TAR0).o

$(TAR1): lib$(TAR0)
	$(CC) $(SRC)/main_$(TAR1).c $(SRC)/rdcheck.c -L $(LIB)/ -l$(TAR0) -I $(INC)/ -o $(TAR)/$(TAR1)

$(TAR2): lib$(TAR0)
	$(CC) $(SRC)/main_$(TAR2).c $(SRC)/rdparser.c -L $(LIB)/ -l$(TAR0)  -I $(INC)/ -o $(TAR)/$(TAR2) -I $(INC)


run_$(TAR1): $(TAR1)
ifeq ($(ALL), 1)
	./$(TAR)/$(TAR1) $(TEST)/
else
	./$(TAR)/$(TAR1)
endif

run_$(TAR2): $(TAR2)
ifeq ($(ALL), 1)
	./$(TAR)/$(TAR2) $(TEST)/
else
	./$(TAR)/$(TAR2)
endif

clean:
	rm -rf $(TAR)/*.o


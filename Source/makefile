compileAndRun: compile

compile: x.lex y.y interpreter.c ast.c sym_table.c
	bison -d y.y
	flex x.lex
	gcc -ll  sym_table.c interpreter.c ast.c lex.yy.c y.tab.c -o ../Builds/camelc

bison: y.y
	bison -d y.y

flex: x.lex y.tab.h
	flex x.lex

clean: lex.yy.c y.tab.h y.tab.c
	rm lex.yy.c y.tab.h y.tab.c


all: gcc -g -c helper.c
	gcc -g -c cache.c
	bison -d parser.y
	flex tokenizer.l
	gcc -g -c processor.c
	cc -pthread -o processor_simulator helper.c cache.c parser.tab.c lex.yy.c -lfl processor.c -lm
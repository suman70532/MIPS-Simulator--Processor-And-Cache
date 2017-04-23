process: tokenizer.l parser.y processor.c
	bison -d parser.y
	flex tokenizer.l
	cc -pthread -o processor_simulator parser.tab.c lex.yy.c -lfl processor.c -lm 

clean:
	rm lex.yy.c parser.tab.c parser.tab.h processor_simulator

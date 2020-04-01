
all: bin tool_output bin/fith

#tool_output/fith_gram.c 
bin/fith: src/fith_compiler.c tool_output/fith_lex.c
	gcc -O2 -s -o bin/fith src/fith_compiler.c -Wall
	size bin/fith

#~ tool_output/fith_gram.c: tool/lemon src/fith_gram.y src/fcompile_gram.y src/ignore_gram.y
#~ 	./tool/lemon src/fith_gram.y -s -dtool_output
#~ 	./tool/lemon src/fcompile_gram.y -s -dtool_output
#~ 	./tool/lemon src/ignore_gram.y -s -dtool_output
#~ 	sed -i 's/void Parse/static void Parse/g' tool_output/fith_gram.c

#~ tool/lemon: tool/lemon.c tool/lempar.c
#~ 	gcc -O2 tool/lemon.c -o tool/lemon

#~ tool/lemon.c:
#~ 	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lemon.c > tool/lemon.c

#~ tool/lempar.c:
#~ 	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lempar.c > tool/lempar.c

tool_output/fith_lex.c: src/fith_lex.re
	re2c -W --eager-skip src/fith_lex.re -o tool_output/fith_lex.c

bin:
	mkdir bin

#~ tool:
#~ 	mkdir tool

tool_output:
	mkdir tool_output

clean:
	rm -f bin/flct
	rm -f tool_output/fith_gram.c
	rm -f tool_output/fith_lex.c

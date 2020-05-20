
all: bin tool_output sqlite3 gen bin/fith fith_src fith_src/script.fith

#tool_output/fith_gram.c 
bin/fith: src/fith_compiler.c src/fith_data.c src/fith_avl.c src/fith_ION.c tool_output/fith_lex.c tool_output/fith_ION_lex.c tool_output/fith_util.o
	./SQLite3_Helper/bin/sqlite3Helper
	gcc -Os -march=native -fno-builtin-strlen -fno-stack-protector -s -o bin/fith src/fith_compiler.c tool_output/fith_util.o -Wall
	size bin/fith

tool_output/fith_lex.c: src/fith_lex.re
	re2c -W --eager-skip src/fith_lex.re -o tool_output/fith_lex.c

tool_output/fith_ION_lex.c: src/fith_ION_lex.re
	re2c -W --eager-skip src/fith_ION_lex.re -o tool_output/fith_ION_lex.c

tool_output/fith_util.o: src/fith_util.c
	gcc -O2 -march=native -fno-stack-protector -c -o tool_output/fith_util.o src/fith_util.c -Wall

# old version
# https://www.sqlite.org/2019/sqlite-autoconf-3300100.tar.gz
#~ sqlite3/sqlite3.o:
#~ 	rm -f sqlite-autoconf-3310100.tar.gz
#~ 	wget "https://www.sqlite.org/2020/sqlite-autoconf-3310100.tar.gz"
#~ 	tar -xvzf sqlite-autoconf-3310100.tar.gz
#~ 	rm -f sqlite-autoconf-3310100.tar.gz
#~ 	(cd sqlite-autoconf-3310100 && ./configure)
#~ 	sed -i 's/-g -O2/-O2/' sqlite-autoconf-3310100/Makefile
#~ 	sed -i 's/-DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_JSON1 -DSQLITE_ENABLE_RTREE/-DNDEBUG -DSQLITE_THREADSAFE=0 -DSQLITE_DQS=0 -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS -DSQLITE_MAX_EXPR_DEPTH=0 -DSQLITE_OMIT_DECLTYPE -DSQLITE_OMIT_DEPRECATED -DSQLITE_OMIT_PROGRESS_CALLBACK -DSQLITE_OMIT_SHARED_CACHE -DSQLITE_ENABLE_JSON1 -DSQLITE_OMIT_AUTOINIT/g' sqlite-autoconf-3310100/Makefile
#~ 	(cd sqlite-autoconf-3310100 && make sqlite3.o)
#~ 	cp sqlite-autoconf-3310100/sqlite3.o sqlite3/sqlite3.o
#~ 	cp sqlite-autoconf-3310100/sqlite3.h sqlite3/sqlite3.h
#~ 	rm -f -R sqlite-autoconf-3310100

#~ SQLite3_Helper/bin/sqlite3Helper:
#~ 	git clone https://github.com/cornerStone-Dev/SQLite3_Helper.git
#~ 	(cd SQLite3_Helper && make child)

bin:
	mkdir bin

gen:
	mkdir gen

#~ sqlite3:
#~ 	mkdir sqlite3

#~ tool:
#~ 	mkdir tool

tool_output:
	mkdir tool_output

fith_src:
	mkdir fith_src

fith_src/script.fith:
	touch fith_src/script.fith

clean:
	rm -f bin/flct
	rm -f tool_output/fith_gram.c
	rm -f tool_output/fith_lex.c


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

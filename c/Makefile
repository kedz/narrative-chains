INC = -I./src/ $(shell pkg-config --cflags glib-2.0 zlib gio-2.0 libxml-2.0 python2)
LIB = -L./obj/ -L./obj/apps/ -L./obj/apps/corenlp-counter/

CFLAGS=-std=c99 -g -Wall -O3  $(INC)
LDLIBS= $(LIB) $(shell pkg-config --libs glib-2.0 zlib gio-2.0 libxml-2.0 python2) -lm 

SRC = src
OBJ = obj
BIN = bin

MKDIR_P = mkdir -p
OBJ_DIRS = $(OBJ)/corenlp $(OBJ)/cooc 

.PHONY: clean directories



#all: tests apps python
all: directories apps

directories:
	$(MKDIR_P) $(OBJ_DIRS)

python: python_build/nchain.so 

apps: directories $(BIN)/corenlp-counter 

$(BIN)/corenlp-counter: corenlp util cooc
	$(CC) $(CFLAGS) $(OBJ)/util.o $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o $(SRC)/apps/corenlp-counter/corenlp-counter.c $(SRC)/apps/corenlp-counter/chambers-nc-counts.c $(SRC)/apps/corenlp-counter/protagonist-nc-counts.c -o $(BIN)/corenlp-counter $(LDLIBS)

    #$(OBJ)/cooc/nchains.o $(OBJ)/apps/corenlp-counter/chambers-nc-counts.o $(SRC)/apps/corenlp-counter/corenlp-counter.c $(SRC)/apps/corenlp-counter/nc_counter.c -o $(BIN)/corenlp-counter $(LDLIBS)

#$(BIN)/corenlp-counter: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/apps/corenlp-counter/nc_counter.o $(OBJ)/cooc/nchains.o $(OBJ)/apps/corenlp-counter/chambers-nc-counts.o $(OBJ)/util.o
#	$(CC) $(CFLAGS) $(OBJ)/util.o $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o $(OBJ)/apps/corenlp-counter/chambers-nc-counts.o $(SRC)/apps/corenlp-counter/corenlp-counter.c $(SRC)/apps/corenlp-counter/nc_counter.c -o $(BIN)/corenlp-counter $(LDLIBS)

#$(OBJ)/apps/corenlp-counter/nc_counter.o:
#	$(CC) $(CFLAGS) -c $(SRC)/apps/corenlp-counter/nc_counter.c -o $(OBJ)/apps/corenlp-counter/nc_counter.o $(LDLIBS)	


#$(OBJ)/apps/corenlp-counter/chambers-nc-counts.o:
#	$(CC) $(CFLAGS) -c $(SRC)/apps/corenlp-counter/chambers-nc-counts.c -o $(OBJ)/apps/corenlp-counter/chambers-nc-counts.o $(LDLIBS)	


#tests: tests/bin/corenlp-document-tests tests/bin/corenlp-xmlparser-tests tests/bin/cooc-nchains-tests
#
#tests/bin/corenlp-document-tests: $(OBJ)/corenlp/document.o tests/obj/corenlp-document-tests.o 
#	$(CC) $(CFLAGS) $(OBJ)/corenlp/document.o tests/obj/corenlp-document-tests.o -o tests/bin/corenlp-document-tests $(LDLIBS)
#
#tests/bin/corenlp-xmlparser-tests: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o tests/obj/corenlp-xmlparser-tests.o
#	$(CC) $(CFLAGS) $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o tests/obj/corenlp-xmlparser-tests.o -o tests/bin/corenlp-xmlparser-tests $(LDLIBS)
#
#tests/bin/cooc-nchains-tests: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o tests/obj/cooc-nchains-tests.o
#	$(CC) $(CFLAGS) $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o tests/obj/cooc-nchains-tests.o -o tests/bin/cooc-nchains-tests $(LDLIBS)
#
#tests/obj/corenlp-document-tests.o: $(OBJ)/corenlp/document.o
#	$(CC) $(CFLAGS) -c tests/src/corenlp-document-tests.c -o tests/obj/corenlp-document-tests.o
#
#tests/obj/corenlp-xmlparser-tests.o: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o
#	$(CC) $(CFLAGS) -c tests/src/corenlp-xmlparser-tests.c -o tests/obj/corenlp-xmlparser-tests.o
#
#tests/obj/cooc-nchains-tests.o: corenlp $(OBJ)/cooc/nchains.o
#	$(CC) $(CFLAGS) -c tests/src/cooc-nchains-tests.c -o tests/obj/cooc-nchains-tests.o
#
corenlp: directories $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o

$(OBJ)/corenlp/xmlparser.o:
	$(CC) $(CFLAGS) -c $(SRC)/corenlp/xmlparser.c -o $(OBJ)/corenlp/xmlparser.o

$(OBJ)/corenlp/document.o:
	$(CC) $(CFLAGS) -c $(SRC)/corenlp/document.c -o $(OBJ)/corenlp/document.o

cooc: directories $(OBJ)/cooc/nchains.o

$(OBJ)/cooc/nchains.o:
	$(CC) $(CFLAGS) -c $(SRC)/cooc/nchains.c -o $(OBJ)/cooc/nchains.o

util: directories $(OBJ)/util.o

$(OBJ)/util.o: directories
	$(CC) $(CFLAGS) -c $(SRC)/util.c -o $(OBJ)/util.o

clean:
	rm -f $(OBJ)/apps/corenlp-counter/*.o $(OBJ)/cooc/*.o $(OBJ)/corenlp/*.o $(OBJ)/*/*/*.o tests/obj/*.o $(OBJ)/*.o tests/bin/* bin/* 

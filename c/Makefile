INC = -I./src/ $(shell pkg-config --cflags glib-2.0 zlib gio-2.0 libxml-2.0)
LIB = -L./obj

CFLAGS=-std=c99 -g -Wall -O3 $(INC)
LDLIBS= $(LIB) $(shell pkg-config --libs glib-2.0 zlib gio-2.0 libxml-2.0) -lm 

SRC = src
OBJ = obj
BIN = bin


all: tests apps

apps: $(BIN)/corenlp-counter 

$(BIN)/corenlp-counter:
	$(CC) $(CFLAGS) $(SRC)/apps/corenlp-counter.c -o $(BIN)/corenlp-counter $(LDLIBS)

tests: tests/bin/corenlp-document-tests tests/bin/corenlp-xmlparser-tests tests/bin/cooc-nchains-tests

tests/bin/corenlp-document-tests: $(OBJ)/corenlp/document.o tests/obj/corenlp-document-tests.o 
	$(CC) $(CFLAGS) $(OBJ)/corenlp/document.o tests/obj/corenlp-document-tests.o -o tests/bin/corenlp-document-tests $(LDLIBS)

tests/bin/corenlp-xmlparser-tests: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o tests/obj/corenlp-xmlparser-tests.o
	$(CC) $(CFLAGS) $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o tests/obj/corenlp-xmlparser-tests.o -o tests/bin/corenlp-xmlparser-tests $(LDLIBS)

tests/bin/cooc-nchains-tests: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o tests/obj/cooc-nchains-tests.o
	$(CC) $(CFLAGS) $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o tests/obj/cooc-nchains-tests.o -o tests/bin/cooc-nchains-tests $(LDLIBS)

tests/obj/corenlp-document-tests.o: $(OBJ)/corenlp/document.o
	$(CC) $(CFLAGS) -c tests/src/corenlp-document-tests.c -o tests/obj/corenlp-document-tests.o

tests/obj/corenlp-xmlparser-tests.o: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o
	$(CC) $(CFLAGS) -c tests/src/corenlp-xmlparser-tests.c -o tests/obj/corenlp-xmlparser-tests.o

tests/obj/cooc-nchains-tests.o: $(OBJ)/corenlp/document.o $(OBJ)/corenlp/xmlparser.o $(OBJ)/cooc/nchains.o
	$(CC) $(CFLAGS) -c tests/src/cooc-nchains-tests.c -o tests/obj/cooc-nchains-tests.o

$(OBJ)/corenlp/xmlparser.o:
	$(CC) $(CFLAGS) -c $(SRC)/corenlp/xmlparser.c -o $(OBJ)/corenlp/xmlparser.o

$(OBJ)/corenlp/document.o:
	$(CC) $(CFLAGS) -c $(SRC)/corenlp/document.c -o $(OBJ)/corenlp/document.o

$(OBJ)/cooc/nchains.o:
	$(CC) $(CFLAGS) -c $(SRC)/cooc/nchains.c -o $(OBJ)/cooc/nchains.o

clean:
	rm -f $(OBJ)/*/*.o tests/obj/*.o tests/bin/* bin/* 

IDIR =include
CC=g++
CFLAGS=-O3 -std=c++1z -I$(IDIR)

ODIR=obj
SDIR=src
LDIR =lib

LIBS=-lgmp

_DEPS = OTPI.h bbs.h terminalFontColors.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o OTPI.o bbs.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

encryptor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
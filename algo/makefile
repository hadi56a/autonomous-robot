SRCDIR = src
SRCLIBDIR = src_lib
LIBDIR = lib
BINDIR = bin
TESTDIR = tests
DOCDIR = doc
INCLUDEDIR = include
INCLUDELIBDIR = include_lib
CC = gcc
DOXYGEN = doxygen
TESTS = tests
CFLAGS = -Wall -pedantic -g
LDFLAGS = -lcunit -L$(LIBDIR) -ldata
EXEC = $(BINDIR)/laby
DOC = $(DOCDIR)/html
MODULES = $(SRCDIR)/labyrinthe.o $(SRCDIR)/robot.o $(SRCDIR)/file.o $(SRCDIR)/robot_pathfinder.o


all : $(EXEC) $(TESTS)

$(TESTS) : $(TESTDIR)/test_labyrinthe $(TESTDIR)/test_robot

$(TESTDIR)/test_% :  $(MODULES) $(LIBDIR)/libdata.a $(SRCDIR)/test_%.o
	$(CC)  -o $@ $^ $(LDFLAGS)


$(EXEC): $(MODULES) $(LIBDIR)/libdata.a $(SRCDIR)/main.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(LIBDIR)/libdata.a : $(SRCLIBDIR)/listeChainee.o $(SRCLIBDIR)/tableHachage.o $(SRCLIBDIR)/copieLiberationComparaisonTypesSimples.o $(SRCLIBDIR)/ListeChaineeListe.o $(SRCLIBDIR)/TableHachageDictionnaire.o  $(SRCLIBDIR)/TableHachageEnsemble.o $(SRCLIBDIR)/graphe.o
	$(AR)  -r $@  $^

$(SRCLIBDIR)/%.o : $(SRCLIBDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I$(INCLUDELIBDIR)

$(SRCDIR)/%.o : $(SRCDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I$(INCLUDEDIR) -I$(INCLUDELIBDIR) -ldata

doc : $(DOCDIR)/index.html

$(DOCDIR)/index.html: $(INCLUDEDIR)/*.h
	$(DOXYGEN)
	cd $(DOCDIR)/latex;make;cd ../..

clean :
	rm -f $(BINDIR)/*
	rm -f $(TESTDIR)/*
	rm -f $(LIBDIR)/*
	rm -f $(SRCDIR)/*.o
	rm -f $(SRCLIBDIR)/*.o
	rm -rf $(DOCDIR)/*

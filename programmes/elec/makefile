CC = gcc
CFLAGS = -Wall -pedantic -g -Iinclude -pthread
LDFLAGS = -lwiringPi -lwiringPiDev -pthread

SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

EXEC = $(BINDIR)/main
EXEC_CALIB = $(BINDIR)/calibrage

SOURCES = $(SRCDIR)/motor.c \
          $(SRCDIR)/mon_lcd.c \
          $(SRCDIR)/capteur_color.c \
          $(SRCDIR)/line.c \
          $(SRCDIR)/buzzer.c \
          $(SRCDIR)/reader.c \
          $(SRCDIR)/utils.c \
          $(SRCDIR)/main.c \
          $(SRCDIR)/calibrage.c
          

COMMON_OBJECTS = $(OBJDIR)/motor.o \
          $(OBJDIR)/mon_lcd.o \
          $(OBJDIR)/capteur_color.o \
          $(OBJDIR)/line.o \
          $(OBJDIR)/buzzer.o \
          $(OBJDIR)/reader.o \
          $(OBJDIR)/utils.o

OBJECTS = $(COMMON_OBJECTS) $(OBJDIR)/main.o

all: $(EXEC) $(EXEC_CALIB)

$(OBJDIR) $(BINDIR):
	mkdir -p $@

$(EXEC): $(OBJECTS) | $(BINDIR)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(EXEC_CALIB): $(COMMON_OBJECTS) $(OBJDIR)/calibrage.o | $(BINDIR)
	$(CC) -o $@ $(COMMON_OBJECTS) $(OBJDIR)/calibrage.o $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

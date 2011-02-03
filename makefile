TARGET = onwire
BINDIR = bin
OBJDIR = build/unix
SRCDIR = src

CPP = g++
CFLAGS = -g0 -O2 -Wall -pipe -I ../lib -I libs -I /usr/include/freetype2 -ftree-vectorize -ftree-vectorizer-verbose=0 -march=native
LDFLAGS = -lGL -lSDL -lfreetype -lpng -lzip
#STATIC_LIBS = /usr/lib/libfreetype.a /usr/lib/libpng.a /usr/lib/libzip.a /usr/lib/libz.a

EXEC = $(BINDIR)/$(TARGET)
OBJ = $(OBJDIR)/OnWireSDL.o \
	$(OBJDIR)/OnWireGame.o \
	$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/**/*.cpp)) \
	$(OBJDIR)/librainbow.a \
	$(OBJDIR)/libBox2D.a

default: check $(EXEC)

$(EXEC): $(OBJ)
	$(CPP) $(LDFLAGS) -o $@ $(OBJ) $(STATIC_LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo Compiling $<
	@$(CPP) -c $< $(CFLAGS) -o $@

%Box2D.a:
	@make -f $(OBJDIR)/makefile.Box2D

%rainbow.a:
	@make -f $(OBJDIR)/makefile.Rainbow

check:
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	@if [ ! -d $(OBJDIR)/Elements ]; then mkdir -p $(OBJDIR)/Elements; fi
	@if [ ! -d $(OBJDIR)/Objects ]; then mkdir -p $(OBJDIR)/Objects; fi

clean:
	@make -f $(OBJDIR)/makefile.Box2D clean
	@make -f $(OBJDIR)/makefile.Rainbow clean
	rm -f $(OBJ)
	rm -fr $(OBJDIR)/Elements
	rm -fr $(OBJDIR)/Objects

clean-all: clean
	@rm -fr $(EXEC)

CFLAGS = -Wall -Wextra -g -Iinclude -Ibuild
LIBS = -lfl -lreadline
BUILD = build
SRC = src
INCLUDE = include
OBJECTS = $(BUILD)/main.o \
	$(BUILD)/lexer.o \
	$(BUILD)/parser.o \
	$(BUILD)/ast.o \
	$(BUILD)/exec.o \
	$(BUILD)/env.o \
	$(BUILD)/front.o

.PHONY : clean
.DEFAULT : all

all : mysh

$(BUILD) :
	mkdir $@

mysh : $(BUILD) $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(BUILD)/lexer.c : $(SRC)/lexer.l
	flex -o $(BUILD)/lexer.c $<

$(BUILD)/parser.c : $(SRC)/parser.y
	bison --verbose --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/parser.h : $(SRC)/parser.y
	bison --verbose --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/lexer.o : $(BUILD)/lexer.c $(BUILD)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/parser.o : $(BUILD)/parser.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/main.c : $(BUILD)/parser.h
$(SRC)/parser.y : $(INCLUDE)/ast.h $(INCLUDE)/env.h $(INCLUDE)/exec.h 
$(SRC)/lexer.l : $(INCLUDE)/front.h
$(SRC)/ast.c : $(INCLUDE)/ast.h
$(SRC)/exec.c : $(INCLUDE)/exec.h $(INCLUDE)/ast.h $(INCLUDE)/env.h
$(SRC)/env.c : $(INCLUDE)/env.h
$(SRC)/front.c : $(INCLUDE)/front.h

clean :
	rm -r $(BUILD) mysh

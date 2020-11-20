CFLAGS = -Wall -Wextra -g
LIBS = -lfl
BUILD = build
SRC = src
INCLUDE = include
OBJECTS = $(BUILD)/main.o \
	$(BUILD)/lexer.o \
	$(BUILD)/parser.o \
	$(BUILD)/ast.o \
	$(BUILD)/exec.o \
	$(BUILD)/env.o

.PHONY : clean
.DEFAULT : all

all : $(BUILD) $(BUILD)/mysh

$(BUILD) :
	mkdir $@

$(BUILD)/mysh : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(BUILD)/lexer.c : $(SRC)/lexer.l
	flex -o $(BUILD)/lexer.c $<

$(BUILD)/parser.c : $(SRC)/parser.y
	bison --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/parser.h : $(SRC)/parser.y
	bison --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/lexer.o : $(BUILD)/lexer.c $(BUILD)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/parser.o : $(BUILD)/parser.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/main.c : $(BUILD)/parser.h
$(SRC)/parser.c : $(INCLUDE)/ast.h
$(SRC)/ast.c : $(INCLUDE)/ast.h
$(SRC)/exec.c : $(INCLUDE)/exec.h $(INCLUDE)/ast.h $(INCLUDE)/env.h
$(SRC)/env.c : $(INCLUDE)/env.h

clean :
	rm -r $(BUILD)

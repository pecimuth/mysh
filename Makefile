CFLAGS = -Wall -Wextra
LIBS = -lfl
BUILD = build
OBJECTS = $(BUILD)/main.o $(BUILD)/lexer.o $(BUILD)/parser.o

.PHONY : clean
.DEFAULT : all

all : $(BUILD) $(BUILD)/mysh

$(BUILD) :
	mkdir $@

$(BUILD)/mysh : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(BUILD)/lexer.c : lexer.l
	flex -o $(BUILD)/lexer.c lexer.l

$(BUILD)/parser.c : parser.y
	bison --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/parser.h : parser.y
	bison --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/lexer.o : $(BUILD)/lexer.c $(BUILD)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/parser.o : $(BUILD)/parser.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/main.o : $(BUILD)/parser.h

clean :
	rm -r $(BUILD)

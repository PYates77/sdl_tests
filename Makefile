LIBS = -lm -lSDL
CFLAGS = 

SRC = 01_draw 02_move 03_transform 04_perspective prender 

% : %.c
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: $(SRC)

clean: 
	rm $(SRC)

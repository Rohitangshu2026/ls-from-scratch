CC = gcc
CFLAGS = -Wall -Wextra 

SRC = myls.c
OBJ = $(SRC:.c=.o)

all: myls

myls: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f myls

re: fclean all

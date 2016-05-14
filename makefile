SHELL = /bin/sh
CC = cc
CFLAGS = -Wall -Werror -g
OBJDIR=obj
vpath %.c src

objects = $(addprefix $(OBJDIR)/, util.o cell.o eval.o builtins.o print.o read.o env.o main.o)

sild: $(objects)
	$(CC) $(CFLAGS) -o sild $(objects)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean run
clean:
	rm -r sild sild.dSYM $(OBJDIR)

run: sild
	./sild

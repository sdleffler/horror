CC = gcc $(GCCFLAGS)
GCCFLAGS = -g -I. -Itest -Isrc

OBJDIR = bin

vpath %.c test
vpath %.h test
vpath %.c src
vpath %.h src

test_objects := $(patsubst test/%.c,bin/%.o,$(wildcard test/*.c))

bin:
	mkdir bin

horror/%.c: horror/macro.h

test/dynarray%.c: horror/dynarray.c

test/heap%.c: horror/heap.c

test/rbtree%.c: horror/rbtree.h horror/rbtree.c

bin/%.o: %.c | bin
	@echo $<
	@$(CC) -c $< -o $@

bin/run_tests: $(test_objects)
	@echo Linking...
	@$(CC) $(test_objects) -o bin/run_tests

test: bin/run_tests
	./bin/run_tests

clean:
	rm -r bin

CC = gcc $(GCCFLAGS)
GCCFLAGS = -g -I. -Itest -Isrc

OBJDIR = bin

vpath %.c test
vpath %.h test

test_objects := $(patsubst test/%.c,bin/%.o,$(wildcard test/*.c))

bin:
	mkdir bin

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

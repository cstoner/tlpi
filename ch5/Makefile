all: append_seek atomic_append dup

append_seek:
	gcc -o append_seek append_seek.c

atomic_append:
	gcc -o atomic_append atomic_append.c

dup:
	gcc -o t_dup dup.c

clean:
	rm -f append_seek atomic_append t_dup

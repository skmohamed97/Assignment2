all:
	gcc -o msh msh.c
run:
	./msh
clean:
	@rm msh
#this makefile is only used for 
#test programs 

all : src/*.c 
	gcc -Og -g -std=gnu17 -o test $^

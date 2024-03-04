#this makefile is only used for 
#test programs 

all : src/*.c 
	gcc -g -Og -o test $^

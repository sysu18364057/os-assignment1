# To run, enter
# make all

all: dph prod cons

dph:dph.cpp
	gcc dph.cpp -lrt -lpthread -o dph
	
prod:prod.cpp
	gcc prod.cpp -lm -lrt -lpthread -o prod
	
cons:cons.cpp
	gcc cons.cpp -lm -lrt -lpthread -o cons

assembler:  assembler.o preprocessor.o first_pass.o second_pass.o symbol_table.o parser.o error_handler.o output_generator.o
	gcc -ansi -Wall -pedantic -g -o assembler assembler.o preprocessor.o first_pass.o second_pass.o symbol_table.o parser.o error_handler.o output_generator.o



assembler.o:assembler.c globals.h preprocessor.h first_pass.h second_pass.h symbol_table.h parser.h error_handler.h output_generator.h
	gcc -ansi -Wall -pedantic -g -c assembler.c


preprocessor.o:  preprocessor.c preprocessor.h globals.h
	gcc -ansi -Wall -pedantic -g -c preprocessor.c

first_pass.o: first_pass.c first_pass.h symbol_table.h globals.h
	gcc -ansi -Wall -pedantic -g -c first_pass.c

second_pass.o: second_pass.c second_pass.h symbol_table.h parser.h error_handler.h globals.h
	gcc -ansi -Wall -pedantic -g -c second_pass.c

symbol_table.o:  symbol_table.c symbol_table.h globals.h
	gcc -ansi -Wall -pedantic -g -c symbol_table.c

parser.o:parser.c parser.h globals.h
	gcc -ansi -Wall -pedantic -g -c parser.c


error_handler.o:  error_handler.c error_handler.h
	gcc -ansi -Wall -pedantic -g -c error_handler.c

output_generator.o:output_generator.c output_generator.h symbol_table.h first_pass.h globals.h
	gcc -ansi -Wall -pedantic -g -c output_generator.c

clean:
	rm -f *.o assembler *.am *.ob *.ent *.ext

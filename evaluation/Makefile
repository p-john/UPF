TRACE=./sources/trace_generator/
DB_GEN=./sources/db_generator/
UPF=./sources/UPF/


all:
	cd $(TRACE);	make trace_generator
	cd $(DB_GEN);	make db_generator
	cd $(UPF);		make -j OPT=-O3
	cp $(TRACE)/trace_generator .
	cp $(DB_GEN)/db_generator .
	cp $(UPF)/bin/UPF .
clean:
	cd $(UPF);		make clean
	rm trace_generator
	rm db_generator
	rm UPF


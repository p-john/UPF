
WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres
$(OPT)? =-Os
$(PG)? = 

CFLAGS = -Wall -pthread -std=c++11 -g -Werror -pedantic-errors -Wextra  $(OPT) $(PG)


INCLUDE = -I./include -I./src -I./test
OBJDIR = obj
OUT = bin/UPF_Tests

UPF_TESTS = $(OBJDIR)/test/test_upf.o \
	$(OBJDIR)/test/test_tokenizer.o \
	$(OBJDIR)/test/test_parser.o \
	$(OBJDIR)/test/test_rulegenerator.o \
	$(OBJDIR)/test/test_upf_manager.o \
	$(OBJDIR)/test/test_utility.o \
	$(OBJDIR)/test/test_dec_tree.o	\
	$(OBJDIR)/test/test_all.o \
	$(OBJDIR)/test/base_test.o \
	$(OBJDIR)/src/UPF.o \
	$(OBJDIR)/src/RuleGenerator.o \
	$(OBJDIR)/src/Parser.o \
	$(OBJDIR)/src/Field.o \
	$(OBJDIR)/src/Tokenizer.o \
	$(OBJDIR)/src/UPF_manager.o \
	$(OBJDIR)/src/utility.o	 \
	$(OBJDIR)/src/dec_tree.o	\
	$(OBJDIR)/src/graph.o	\
	$(OBJDIR)/src/fdd.o		\
	$(OBJDIR)/src/UPF_ruleset.o\
	$(OBJDIR)/src/UPF_exporter.o \
	$(OBJDIR)/src/saxpac.o \
	$(OBJDIR)/src/fw_compressor.o \
	$(OBJDIR)/src/UPF_optimizations.o

UPF = $(OBJDIR)/src/UPF.o \
	$(OBJDIR)/src/RuleGenerator.o \
	$(OBJDIR)/src/Parser.o \
	$(OBJDIR)/src/Field.o \
	$(OBJDIR)/src/Tokenizer.o \
	$(OBJDIR)/src/UPF_manager.o \
	$(OBJDIR)/src/utility.o		\
	$(OBJDIR)/src/main.o 		\
	$(OBJDIR)/src/dec_tree.o	\
	$(OBJDIR)/src/graph.o	\
	$(OBJDIR)/src/fdd.o		\
	$(OBJDIR)/src/UPF_ruleset.o \
	$(OBJDIR)/src/UPF_exporter.o \
	$(OBJDIR)/src/saxpac.o \
	$(OBJDIR)/src/fw_compressor.o \
	$(OBJDIR)/src/UPF_optimizations.o

	
all: before build

before: 
	test -d bin || mkdir -p bin
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	test -d $(OBJDIR)/test || mkdir -p $(OBJDIR)/test
	test -d $(OBJDIR)/src || mkdir -p $(OBJDIR)/src
	
build: $(UPF_TESTS) $(UPF)
	$(LD) -o $(OUT) $(UPF_TESTS)  $(LDFLAGS_DEBUG) $(LIB_DEBUG) -lpthread $(PG)
	$(LD) -o bin/UPF $(UPF) $(LDFLAGS_DEBUG) $(LIB_DEBUG) -lpthread $(PG)

UPF: before $(UPF)
	$(LD) -o bin/UPF $(UPF) $(LDFLAGS_DEBUG) $(LIB_DEBUG) -lpthread $(PG)

	
$(OBJDIR)/test/test_upf.o: test/test_upf.cpp
	$(CXX)  $(CFLAGS) $(INCLUDE) -c test/test_upf.cpp -o  $(OBJDIR)/test/test_upf.o 
	
$(OBJDIR)/test/test_parser.o: test/test_parser.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_parser.cpp -o $(OBJDIR)/test/test_parser.o 

$(OBJDIR)/test/test_tokenizer.o: test/test_tokenizer.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_tokenizer.cpp -o $(OBJDIR)/test/test_tokenizer.o 
	
$(OBJDIR)/test/test_logger.o: test/test_logger.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_logger.cpp -o $(OBJDIR)/test/test_logger.o 
	
$(OBJDIR)/test/test_rulegenerator.o: test/test_rulegenerator.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_rulegenerator.cpp -o $(OBJDIR)/test/test_rulegenerator.o 
	
$(OBJDIR)/test/test_upf_manager.o: test/test_upf_manager.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_upf_manager.cpp -o $(OBJDIR)/test/test_upf_manager.o 

$(OBJDIR)/test/test_utility.o: test/test_utility.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_utility.cpp -o $(OBJDIR)/test/test_utility.o 

$(OBJDIR)/test/test_dec_tree.o: test/test_dec_tree.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_dec_tree.cpp -o $(OBJDIR)/test/test_dec_tree.o 

$(OBJDIR)/test/test_graph.o: test/test_graph.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_graph.cpp -o $(OBJDIR)/test/test_graph.o 
	
$(OBJDIR)/test/test_all.o: test/test_all.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/test_all.cpp -o $(OBJDIR)/test/test_all.o 

$(OBJDIR)/test/base_test.o: test/base_test.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c test/base_test.cpp -o $(OBJDIR)/test/base_test.o 

$(OBJDIR)/src/UPF.o: src/UPF.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/UPF.cpp -o $(OBJDIR)/src/UPF.o

$(OBJDIR)/src/RuleGenerator.o: src/RuleGenerator.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/RuleGenerator.cpp -o $(OBJDIR)/src/RuleGenerator.o 
	
$(OBJDIR)/src/Parser.o: src/Parser.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/Parser.cpp -o $(OBJDIR)/src/Parser.o 

$(OBJDIR)/src/Field.o: src/Field.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/Field.cpp -o $(OBJDIR)/src/Field.o 
	
$(OBJDIR)/src/Tokenizer.o: src/Tokenizer.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/Tokenizer.cpp -o $(OBJDIR)/src/Tokenizer.o 
		
$(OBJDIR)/src/UPF_manager.o: src/UPF_manager.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/UPF_manager.cpp -o $(OBJDIR)/src/UPF_manager.o 

$(OBJDIR)/src/utility.o: src/utility.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/utility.cpp -o $(OBJDIR)/src/utility.o 

$(OBJDIR)/src/main.o: src/main.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/main.cpp -o $(OBJDIR)/src/main.o 

$(OBJDIR)/src/dec_tree.o: src/dec_tree.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/dec_tree.cpp -o $(OBJDIR)/src/dec_tree.o 

$(OBJDIR)/src/graph.o: src/graph.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/graph.cpp -o $(OBJDIR)/src/graph.o 

$(OBJDIR)/src/fdd.o: src/fdd.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/fdd.cpp -o $(OBJDIR)/src/fdd.o 	

$(OBJDIR)/src/UPF_ruleset.o: src/UPF_ruleset.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/UPF_ruleset.cpp -o $(OBJDIR)/src/UPF_ruleset.o 		
	
$(OBJDIR)/src/UPF_exporter.o: src/UPF_exporter.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/UPF_exporter.cpp -o $(OBJDIR)/src/UPF_exporter.o 
	
$(OBJDIR)/src/saxpac.o: src/saxpac.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/saxpac.cpp -o $(OBJDIR)/src/saxpac.o 

$(OBJDIR)/src/fw_compressor.o: src/fw_compressor.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/fw_compressor.cpp -o $(OBJDIR)/src/fw_compressor.o 
	
$(OBJDIR)/src/UPF_optimizations.o: src/UPF_optimizations.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c src/UPF_optimizations.cpp -o $(OBJDIR)/src/UPF_optimizations.o 
	
clean: 
	rm -f $(OBJ) $(OUT)
	rm -rf bin
	rm -rf $(OBJDIR)
	rm -rf $(OBJDIR)/test
	rm -rf $(OBJDIR)/src


MAKEFLAGS += --no-print-directory

#INC_DIR=weilei_lib
INC_DIR=.
#INC_DIR=~/working/weilei_lib
CXX=g++ -O3 -Wall -std=c++11
#-fPIC for dynamic lib
### -O2 -O5 -Os
#g++ `pkg-config --cflags itpp` -o hello.out hello.cpp `pkg-config --libs itpp`

#START=`pkg-config --cflags itpp`
#END=`pkg-config --libs itpp`

ITPP=`pkg-config --cflags itpp` `pkg-config --libs itpp`

#files=$(INC_DIR)/mm_read.cpp $(INC_DIR)/mm_read.h $(INC_DIR)/mmio.c $(INC_DIR)/mmio.h $(INC_DIR)/mm_write.cpp $(INC_DIR)/mm_write.h $(INC_DIR)/lib.cpp $(INC_DIR)/lib.h $(INC_DIR)/dist.cpp $(INC_DIR)/dist.h $(INC_DIR)/concatenation_lib.cpp $(INC_DIR)/concatenation_lib.h  $(INC_DIR)/bp.cpp $(INC_DIR)/bp.h $(INC_DIR)/my_lib.h Makefile
#files=$(INC_DIR)/mm_read.cpp $(INC_DIR)/mm_read.h $(INC_DIR)/mmio.c $(INC_DIR)/mmio.h $(INC_DIR)/mm_write.cpp $(INC_DIR)/mm_write.h $(INC_DIR)/lib.cpp $(INC_DIR)/lib.h $(INC_DIR)/dist.cpp $(INC_DIR)/dist.h $(INC_DIR)/product.cpp $(INC_DIR)/product.h  $(INC_DIR)/bp.cpp $(INC_DIR)/bp.h $(INC_DIR)/weilei_lib.h Makefile
#command=$(CXX) $(START) -o $@ $< $(word 2,$^) $(word 4, $^) $(word 6, $^) $(word 8, $^) $(word 10, $^) $(word 12, $^) $(word 14, $^) $(END)
###include all weilei-written headfiles into weilei_lib.h


header_files=bp.h      bp_decoder.h      dist.h      lib.h      mm_read.h      mm_write.h      mmio.h      product_lib.h      weilei_lib.h

object_files=$(INC_DIR)/mm_read.o $(INC_DIR)/mmio.o $(INC_DIR)/mm_write.o $(INC_DIR)/lib.o $(INC_DIR)/dist.o $(INC_DIR)/product_lib.o $(INC_DIR)/bp.o 


all: morehead head cpp
morehead: mmio.h.gch mm_read.h.gch mm_write.h.gch dist.h.gch bp.h.gch lib.h.gch product_lib.h.gch
cpp: mmio.o mm_read.o mm_write.o dist.o bp.o lib.o product_lib.o
head: bp_decoder.h.gch weilei_lib.h.gch

#not sure why mmio rebuild every time, but it works fine with `make mmio.o`
mmio.o:mmio.c mmio.h
	$(CXX) -c $< -o mmio.o
#compile object file for cpp 
%.o:%.cpp %.h weilei_lib.h
	$(CXX) $(ITPP) -c $<
#compile object file for headfile; this can have errors, as long as the .o file can be built without error
%.h.gch:%.h weilei_lib.h
	$(CXX) $(ITPP) -c $<

#test the lib
test_lib.o:test_lib.cpp weilei_lib.h $(header_files)
	$(CXX) $(ITPP) -c $<
test:
	make all
	make test_lib.o
	make test_lib.out
	./test_lib.out
test_lib.out:test_lib.o $(object_files)
	$(CXX) $(ITPP) -o $@ $< $(object_files)

#build dynamic lib
# require -fPIC option
LIB_WEILEI_PATH=/rhome/wzeng002/.local/lib
libweilei.so:$(object_files)
	make all
	$(CXX) $(ITPP) -shared -o $@ $(object_files)
	cp libweilei.so $(LIB_WEILEI_PATH)
#    $(CXX) -fPIC -c shared.cpp -o shared.o
#    $(CXX) -shared  -Wl,-soname,libshared.so -o libshared.so shared.o
dynamic_lib:libweilei.so test_lib.o
	make libweilei.so
	$(CXX) $(ITPP) -o test_dynamic_lib.out  test_lib.o -L$(LIB_WEILEI_PATH) -lweilei
	./test_dynamic_lib.out

clean:
	rm *.o
	rm *.h.gch
	rm *.out

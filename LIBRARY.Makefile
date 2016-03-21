LIB    = $(shell basename $(shell pwd))
CC     = g++
CFLAGS = $(ARCH) -O2 -Wundef -DLINUX -I /home/pisti/projects/goldminer/gitbuli/GoldMiner/jusText/htmlcxx-0.84
#CFLAGS = $(ARCH) -O2 -Wundef -DLINUX -I ../htmlcxx-0.84
OBJS   = $(wildcard *.cc) $(wildcard *.cpp) $(wildcard *.c)

HAVEMAKE = $(wildcard Makefile)
include $(HAVEMAKE)

$(OUTPUT)%.o: %
	@echo "  compile $(LIB): $(shell basename $@)"
	@if [ ! -e $(OUTPUT) ]; then mkdir -p $(OUTPUT); fi
	@$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -c $< -o $@

obj: $(OBJS:%=$(OUTPUT)%.o)
	@echo "  compile $(LIB): done"

lib: obj
	ar rcs $(OUTPUT)lib$(LIB).a $(OBJS:%=$(OUTPUT)%.o)

clean:
	rm -f $(OUTPUT)*.o $(OUTPUT)*.a

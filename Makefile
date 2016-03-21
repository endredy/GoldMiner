CC     = g++
PWD    = $(shell pwd)
OUTPUT = $(shell pwd)
OUTDIR = .
CFLAGS = $(ARCH) -c -DNDEBUG -DHAVE_CONFIG_H -fPIC -DLINUX -I ../jusText/htmlcxx-0.84 -DPCRE_STATIC `xml2-config --cflags` -DSQLITE_THREADSAFE=1 -DCIKKHEZ
LDFLAGS= $(ARCH) -pthread `xml2-config --libs`
LDLIBS=-lcurl -lpcrecpp -lpcre 
MYLIBS=
# -LURLProc  -LMidCharConverter -LMinCharConverter 

EXT_LIBS = /jusText/htmlcxx-0.84/html	\
	/src

JUSTEXTMODULE=jusText/parseFSM.cpp jusText/justext.cpp jusText/tools.cpp

#/CharConverter<----->
# /MmoInput 
#    /URLProc<-->\
#  /DatabaseWrapper/MySQL
all: $(EXT_LIBS) $(OUTDIR)/jusTextTest $(OUTDIR)/goldMinerTest
#	@$(CC) $(LDFLAGS) -o $(OUTPUT)/$(LIB)$(ARCH).bin $(EXT_LIBS:%=$(OUTPUT)%/*.o)

clean:
#	-rm -f $(OBJECTS)/*.o 
	@rm -f -r $(OUTPUT)/*/*.o $(OUTDIR)/goldMinerTest

$(EXT_LIBS):
	@make -C .$@ -f $(PWD)/LIBRARY.Makefile obj lib ARCH=$(ARCH) OUTPUT=$(OUTPUT)$@/ CFLAGS="$(CFLAGS)"

$(OUTDIR)/jusTextTest:
	g++ jusText/test.cpp $(JUSTEXTMODULE) -I jusText/htmlcxx-0.84 -o $(OUTDIR)/jusTextTest  $(EXT_LIBS:%=$(OUTPUT)%/*.o) -Wl,-static -Wl,-Bdynamic $(LDFLAGS) $(LDLIBS) $(MYLIBS)


$(OUTDIR)/goldMinerTest:
	g++ $(JUSTEXTMODULE) -I jusText/htmlcxx-0.84 -o $(OUTDIR)/goldMinerTest  $(EXT_LIBS:%=$(OUTPUT)%/*.o) -Wl,-static -Wl,-Bdynamic $(LDFLAGS) $(LDLIBS) $(MYLIBS)


CC     = g++
PWD    = $(shell pwd)
OUTPUT = $(shell pwd)
OUTDIR = .
CFLAGS = $(ARCH) -c -DNDEBUG -DHAVE_CONFIG_H -fPIC -DLINUX -I ../jusText/htmlcxx-0.84 -DPCRE_STATIC `xml2-config --cflags`
LDFLAGS= $(ARCH) -pthread `xml2-config --libs`
LDLIBS=-lpcrecpp -lpcre 
MYLIBS=

EXT_LIBS_JUSTEXT = /jusText/htmlcxx-0.84/html
EXT_LIBS = /jusText/htmlcxx-0.84/html	\
	/src

JUSTEXTMODULE=jusText/parseFSM.cpp jusText/justext.cpp jusText/tools.cpp

all: $(EXT_LIBS) $(OUTDIR)/goldMinerTest

clean:
	@rm -f -r $(OUTPUT)/*/*.o $(OUTDIR)/goldMinerTest

$(EXT_LIBS):
	@make -C .$@ -f $(PWD)/LIBRARY.Makefile obj lib ARCH=$(ARCH) OUTPUT=$(OUTPUT)$@/ CFLAGS="$(CFLAGS)"

#$(OUTDIR)/jusTextTest:
#	g++ jusText/test.cpp $(JUSTEXTMODULE) -I jusText/htmlcxx-0.84 -o $(OUTDIR)/jusTextTest  $(EXT_LIBS_JUSTEXT:%=$(OUTPUT)%/*.o) -Wl,-static -Wl,-Bdynamic $(LDFLAGS) $(LDLIBS) $(MYLIBS)


$(OUTDIR)/goldMinerTest:
	g++ $(JUSTEXTMODULE) -I jusText/htmlcxx-0.84 -o $(OUTDIR)/goldMinerTest  $(EXT_LIBS:%=$(OUTPUT)%/*.o) -Wl,-static -Wl,-Bdynamic $(LDFLAGS) $(LDLIBS) $(MYLIBS)


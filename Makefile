CC= gcc
RM= /bin/rm -vf
ARCH=UNDEFINED
PWD=pwd
CDR=$(shell pwd)

EDCFLAGS:=$(CFLAGS)
EDLDFLAGS:=$(LDFLAGS)

ifeq ($(ARCH),UNDEFINED)
	ARCH=$(shell uname -m)
endif

EDCFLAGS:= -Wall -fno-strict-aliasing -std=gnu11 -O2 $(EDCFLAGS)
EDLDFLAGS:= -lm -lpthread $(EDLDFLAGS)

EDCFLAGS+= -Wno-unused-result -Wno-format

TARGETOBJS=src/datalogger.o \
			src/datalogger_test.o

TARGET=datalogger_tester.out

all: build/$(TARGET)

build:
	mkdir build

build/$(TARGET): $(TARGETOBJS) build
	$(CC) $(TARGETOBJS) $(LINKOPTIONS) -o $@ \
	$(EDLDFLAGS)

%.o: %.c
	$(CC) $(EDCFLAGS) -Iinclude/ -Idrivers/ -o $@ -c $<

# clean: cleanobjs
clean:
	$(RM) build/$(TARGET)
	$(RM) $(TARGETOBJS)

spotless: clean
	$(RM) *.tmp
	$(RM) *.idx
	$(RM) *.reg
	$(RM) *.log
	$(RM) -R build
	$(RM) -R docs

run: build/$(TARGET)
	sudo build/$(TARGET)

doc:
	doxygen .doxyconfig

pdf: doc
	cd docs/latex && make && mv refman.pdf ../../
# cleanobjs:
# 	find $(CDR) -name "*.o" -type f
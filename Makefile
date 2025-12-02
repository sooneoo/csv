PLATFORM ?= linux_x86_64

TARGET=libcsv.a
CACHE=.cache
RELEASE=$(CACHE)/release



all: build


-include config/$(PLATFORM).mk


build: env $(RELEASE)/$(TARGET)


.PHONY: env dep clean install


exec: build $(RELEASE)/test  
	$(RELEASE)/test


install:
	cp -v $(RELEASE)/$(TARGET) $(LIBDIR)/$(TARGET)
	cp -vr src/include/csv $(INDIR)


env:
	mkdir -pv $(CACHE)
	mkdir -pv $(RELEASE)


clean: 
	rm -rvf $(CACHE)




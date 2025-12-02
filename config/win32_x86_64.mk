

SRC=src/include

CC=gcc
CFLAGS += -Wall 
CFLAGS += -Wextra 
CFLAGS += -pedantic 
CFLAGS += -O2
CFLAGS += -I$(SRC)

LIBS += -lalloc


OBJS += $(CACHE)/csv.o


INDIR = /usr/include/csv
LIBDIR = /usr/lib


$(CACHE)/csv.o: src/csv/csv.c
	$(CC) $(CFLAGS) -c $< -o $@


$(RELEASE)/$(TARGET): $(OBJS)
	ar -crs $(RELEASE)/$(TARGET) $(OBJS)



TEST += $(CACHE)/test.o


$(CACHE)/test.o: test/test.c 
	$(CC) $(CFLAGS) -c $< -o $@


$(RELEASE)/test: env  $(TEST)
	$(CC) $(TEST) $(LIBS) -L$(RELEASE) -lcsv -o $(RELEASE)/test



CC=gcc
VERSION=v0.1.0
CFLAGS=-std=gnu99 -Wall
#CFLAGS=-std=gnu99 -fsanitize=address -Wall
LIBS=-lm

TARGET=gifmetadata
LIBTARGET=libgifmetadata.a

OBJS = main.o cli.o
LIBOBJS = gifmetadata.o gif.o

all: $(TARGET)

$(LIBTARGET): $(LIBOBJS)
	ar rcs $(LIBTARGET) $(LIBOBJS)

$(TARGET): $(OBJS) $(LIBTARGET)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) -L . -lgifmetadata -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.core *.tar.gz $(TARGET) $(LIBTARGET)

install: gifmetadata
	sudo install -m 755 gifmetadata /usr/local/bin

uninstall:
	sudo rm /usr/local/bin/gifmetadata

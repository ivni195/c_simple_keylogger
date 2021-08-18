CC=gcc
IDIR=../include
CFLAGS=-I$(IDIR)
ODIR=obj

_DEPS=event_monitor.h input_dev_utils.h client.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=client.o event_monitor.o input_dev_utils.o main.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

keylogger: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f obj/* keylogger

$(ODIR):
	mkdir -p $@

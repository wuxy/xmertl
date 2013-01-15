##################################################################
#                    Customisable section                        #
##################################################################

#### Name of the target file.
TARGET        := application.prtk

#### Path to the PaRTiKle root (it can be relative or absolute)
PRTK := ../../../../partikle

#### All the ``C'' files of the current folder are part of the application.
#### SOURCES can be a explicit list of files.
SOURCES       := $(wildcard *.c)

#### Commen the followin line to compile quietly.
#V             := 1

##################################################################
#                   Not customisable section                     #
##################################################################

OBJECTS       := $(addsuffix .o, $(basename $(SOURCES)))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LDKERNEL) -o $@  $^

%o: %c
	$(CC) $(CFLAGS) -c -o $@  $<

clean:
	rm -f *.prtk *.ktr *.o *~

include $(PRTK)/config.mk
include $(PRTK)/user/rules.mk




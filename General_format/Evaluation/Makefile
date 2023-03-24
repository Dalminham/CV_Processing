CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar rcs
CFLAGS=  -O3   -ggdb -g
LIBS= -lm


INCS=
SRCS:=$(wildcard *.c ../Utils/*.c )
SRCS_S:=$(wildcard *.S)
COBJS:=$(SRCS:.c=.o)
COBJS_S:=$(SRCS_S:.S=.o)

# $(warning "$(SRCS)" )
# $(warning "$(SRCS_S)" )
# $(warning "$(COBJS)")
# $(warning "$(COBJS_S)")

all:lib main

$(COBJS) : %.o: %.c
	$(CC) -c $< -o $@ $(INCS) $(CFLAGS) $(LIBS)

$(COBJS_S) : %.o: %.S
	$(CC) -c $< -o $@ $(INCS) $(CFLAGS) $(LIBS)

lib:$(COBJS) $(COBJS_S)
	$(AR) -o libalgorithm.a $(COBJS)

main:$(SRCS) $(SRCS_S)
	$(CC) $(COBJS) $(COBJS_S) $(LIBS) $(CFLAGS) -o main
	
clean:
	rm -rf *.o main lib* ../Utils/*.o  ../Utils/lib*

clean_bmp:
	rm `ls *.bmp | egrep -v in.bmp`


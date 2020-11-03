GXX := g++
RM := rm -rf

SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc, %.o, $(SRCS))
ELF = dict

all : $(ELF)
$(ELF) : $(OBJS)
	$(GXX) $^ -o $@ -lpthread

.PHONY : rebuild clean
rebuild : clean all
clean :
	$(RM) $(OBJS) $(ELF)

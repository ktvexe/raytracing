EXEC = raytracing
.PHONY: all
all: $(EXEC)

CC ?= gcc
CFLAGS = \
	-std=gnu99 -Wall -O0 -g 
LDFLAGS = \
	-lm -lpthread 

ifeq ($(strip $(PROFILE)),1)
PROF_FLAGS = -pg
CFLAGS += $(PROF_FLAGS)
LDFLAGS += $(PROF_FLAGS) 
endif

OBJS := \
	objects.o \
	raytracing.o \
	main.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: use-models.h
use-models.h: models.inc Makefile
	@echo '#include "models.inc"' > use-models.h
	@egrep "^(light|sphere|rectangular) " models.inc | \
	    sed -e 's/^light /append_light/g' \
	        -e 's/light[0-9]/(\&&, \&lights);/g' \
	        -e 's/^sphere /append_sphere/g' \
	        -e 's/sphere[0-9]/(\&&, \&spheres);/g' \
	        -e 's/^rectangular /append_rectangular/g' \
	        -e 's/rectangular[0-9]/(\&&, \&rectangulars);/g' \
	        -e 's/ = {//g' >> use-models.h

calculate: calculate.c
	$(CC) $(CFLAGS) $^ -o $@

output.txt: run calculate
	./calculate

plot:runtime.txt
	gnuplot scripts/runtime.gp


n ?=10

run:
	n=$(n); \
   while [ $${n} -gt 0 ] ; do \
       ./raytracing ; \
       n=`expr $$n - 1`; \
   done; \
   true

clean:
	$(RM) $(EXEC) $(OBJS) use-models.h \
		out.ppm gmon.out opt.txt

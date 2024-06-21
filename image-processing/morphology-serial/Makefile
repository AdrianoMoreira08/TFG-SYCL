#===============================================================================
# User Options
#===============================================================================

# Compiler can be set below, or via environment variable
CC        = clang++
OPTIMIZE  = yes
DEBUG     = no

#===============================================================================
# Program name & source code list
#===============================================================================

program = morph

source = main.cc morphology.cc fits_image.cc
incl = morphology.h fits_image.h

obj = $(source:.cc=.o)

prefixed_obj = $(addprefix build/,$(obj))
prefixed_incl = $(addprefix include/,$(incl))

#===============================================================================
# Sets Flags
#===============================================================================

# Standard Flags
CFLAGS := $(EXTRA_CFLAGS) -std=c++17 -Wall

# Linker Flags
PREVLDFLAGS = -Wl,-rpath,/home/verode/alu0101436784/soft/cfitsio-4.4.0/lib
LDFLAGS = -L/home/verode/alu0101436784/soft/cfitsio-4.4.0/lib -lcfitsio -lm -lz

OBJFLAGS = -I/home/verode/alu0101436784/soft/cfitsio-4.4.0/include -Dg77Fortran -fPIC

# Debug Flags
ifeq ($(DEBUG),yes)
  CFLAGS  += -g -DDEBUG
  LDFLAGS += -g
endif

# Optimization Flags
ifeq ($(OPTIMIZE),yes)
  CFLAGS += -O3
endif

#===============================================================================
# Targets to Build
#===============================================================================

$(program): $(obj)
	$(CC) $(CFLAGS) $(PREVLDFLAGS) $(prefixed_obj) -I. -o bin/$@ $(LDFLAGS)

%.o: src/%.cc
	$(CC) $(CFLAGS) -c $(OBJFLAGS) $< -o build/$@

clean:
	rm -rf bin/$(program) build/$(obj)

template:
	mkdir bin build

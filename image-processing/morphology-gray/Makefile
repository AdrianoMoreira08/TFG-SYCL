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

source = main.cc \
				 fits_image.cc \
				 fits_utils.cc \
				 templated_fits_image.cc \
				 structuring_element.cc \
				 templated_structuring_element.cc \
			 	 morphology.cc \
			 	 erode.cc \
				 utils.cc
incl = fits_image.h \
			 fits_utils.h \
			 templated_fits_image.h \
			 structuring_element.h \
			 templated_structuring_element.h \
			 morphology.h \
			 erode.h \
			 utils.h

obj = $(source:.cc=.o)

prefixed_obj = $(addprefix build/,$(obj))
prefixed_incl = $(addprefix include/,$(incl))

#===============================================================================
# Sets Flags
#===============================================================================

# Standard Flags
CFLAGS := $(EXTRA_CFLAGS) -std=c++17 -Wall

# Linker Flags
PREVLDFLAGS = -Wl,-rpath,/home/verode/alu0101436784/soft/cfitsio-3.4.5/lib
LDFLAGS = -L/home/verode/alu0101436784/soft/cfitsio-3.4.5/lib -lcfitsio -lm -lz

OBJFLAGS = -I/home/verode/alu0101436784/soft/cfitsio-3.4.5/include -Dg77Fortran -fPIC

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

.PHONY: template clean

bin/$(program): $(prefixed_obj)
	$(CC) $(CFLAGS) $(PREVLDFLAGS) $(prefixed_obj) -I. -o $@ $(LDFLAGS)

build/%.o: src/%.cc
	$(CC) $(CFLAGS) -c $(OBJFLAGS) $< -o $@

template:
	mkdir bin build

clean:
	rm -rf bin/$(program) $(prefixed_obj)

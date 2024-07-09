#===============================================================================
# User Options
#===============================================================================

# Compiler can be set below, or via environment variable
CC           = clang++
OPTIMIZE     = yes
DEBUG        = no
SYCL         = no
CFITSIO_PATH = "/home/verode/alu0101436784/soft/cfitsio-3.4.5"

SYCL_CUDA     = no
CUDA_ARCH     = sm_70
GCC_TOOLCHAIN = "/home/bejeque/acabrera/soft/gcc/12.3.0"

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
			 utils.h

obj = $(source:.cc=.o)

ifeq ($(SYCL),yes)
	program := $(program)_sycl
	incl += erode_sycl.h
	obj := $(addprefix sycl_,$(obj))
	OBJ_PREFIX := sycl_
	CFLAGS +=-DUSE_SYCL
else
	incl += erode.h
endif

prefixed_obj = $(addprefix build/,$(obj))
prefixed_incl = $(addprefix include/,$(incl))

#===============================================================================
# Sets Flags
#===============================================================================

# Standard Flags
CFLAGS := $(CFLAGS) $(EXTRA_CFLAGS) -std=c++17 -Wall

# Linker Flags
PREVLDFLAGS = -Wl,-rpath,$(CFITSIO_PATH)/lib
LDFLAGS = -L$(CFITSIO_PATH)/lib -lcfitsio -lm -lz

OBJFLAGS = -I$(CFITSIO_PATH)/include -Dg77Fortran -fPIC

# Debug Flags
ifeq ($(DEBUG),yes)
  CFLAGS  += -g -DDEBUG
  LDFLAGS += -g
endif

ifeq ($(SYCL_CUDA), yes)
  CFLAGS += -fsycl -fsycl-targets=nvptx64-nvidia-cuda \
            -Xsycl-target-backend --cuda-gpu-arch=$(CUDA_ARCH) \
            --gcc-toolchain=$(GCC_TOOLCHAIN)
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

build/$(OBJ_PREFIX)%.o: src/%.cc
	$(CC) $(CFLAGS) -c $(OBJFLAGS) $< -o $@

template:
	mkdir bin build

clean:
	rm -rf bin/* build/*

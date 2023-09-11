
# = = = = = = = = = = = = = = = = = = = = = = = = = = = #
# Determine the correct value for these variables for
# your system and export them to your environment or
# define them here.

CXX ?= clang++

# Binsparse (this should be fine unless you do an
# out-of-source build)
BINSPARSE_DIR ?= ../include

# HDF5 library location
HDF5_DIR ?= /opt/homebrew/Cellar/hdf5/1.14.2
HDF5_CXXFLAGS ?= -I$(HDF5_DIR)/include
HDF5_LIBRARY_FLAGS ?= -L$(HDF5_DIR)/lib -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5 

# = = = = = = = = = = = = = = = = = = = = = = = = = = = #

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

CXXFLAGS = -std=c++20 -O3 -I$(BINSPARSE_DIR)

CXXFLAGS += $(HDF5_CXXFLAGS)
LD_FLAGS += $(HDF5_LIBRARY_FLAGS)

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -fv $(TARGETS)

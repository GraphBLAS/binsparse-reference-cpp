
CXX = g++-12

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

BINSPARSE_DIR=../include

CXXFLAGS = -std=c++20 -O3 -I$(BINSPARSE_DIR)

# Update HDF5 Flags for your HDF5 installation
HDF5_CXXFLAGS ?= -I/opt/homebrew/Cellar/hdf5/1.12.2_2/include
HDF5_LD_FLAGS ?= -L/opt/homebrew/Cellar/hdf5/1.12.2_2/lib -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5 

CXXFLAGS += $(HDF5_CXXFLAGS)
LD_FLAGS += $(HDF5_LD_FLAGS)

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -fv $(TARGETS)

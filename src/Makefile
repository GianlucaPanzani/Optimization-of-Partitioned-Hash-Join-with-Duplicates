CXX            = g++ -std=c++17
OPTFLAGS       = -O3 -march=native
AVXFLAGS       = -mavx2
AUTOFLAGS      = -fopt-info-vec-optimized -fopt-info-vec-missed
NOVECFLAGS     = -fno-tree-vectorize
CXXFLAGS      += -Wall
INCLUDES       = -I. -I./include
LIBS           =

SOURCES        = $(wildcard *.cpp)

TARGETS        = generate_datasets plain_novec plain_vec avx2

.PHONY: all clean cleanall report

all: $(TARGETS)

generate_datasets: generate_datasets.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LIBS)

plain_novec: plain.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(NOVECFLAGS) -o $@ $< $(LIBS)

plain_vec: plain.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ $< $(LIBS) 2> gcc_vectorization_report.txt

avx2: avx2.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AVXFLAGS) -o $@ $< $(LIBS)

report: plain.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o plain_vec $< $(LIBS) 2> gcc_vectorization_report.txt

launch-benchmark: LIBS += -pthread

clean:
	-rm -fr *.o *~ gcc_vectorization_report.txt

cleanall: clean
	-rm -fr $(TARGETS)
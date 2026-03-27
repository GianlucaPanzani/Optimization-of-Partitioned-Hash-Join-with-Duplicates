CXX            = g++ -std=c++17
OPTFLAGS       = -O3 -march=native
AVXFLAGS       = -mavx2
AUTOFLAGS      = -fopt-info-vec-optimized -fopt-info-vec-missed
NOVECFLAGS     = -fno-tree-vectorize
CXXFLAGS      += -Wall
INCLUDES       = -I. -I./include
LIBS           =

LIB_SOURCES    = lib/dataset.cpp lib/timing.cpp lib/checksum.cpp lib/metrics.cpp lib/partition.cpp

TARGETS        = generate_datasets plain_novec plain_vec #avx2

.PHONY: all clean cleanall report

all: $(TARGETS)

generate_datasets: generate_datasets.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ generate_datasets.cpp $(LIB_SOURCES) $(LIBS)

plain_novec: plain.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(NOVECFLAGS) -o $@ plain.cpp $(LIB_SOURCES) $(LIBS)

plain_vec: plain.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ plain.cpp $(LIB_SOURCES) $(LIBS) 2> gcc_vectorization_report.txt

avx2: avx2.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AVXFLAGS) -o $@ avx2.cpp $(LIB_SOURCES) $(LIBS)

report: plain.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o plain_vec plain.cpp $(LIB_SOURCES) $(LIBS) 2> gcc_vectorization_report.txt

launch-benchmark: LIBS += -pthread

clean:
	-rm -fr *.o *~ gcc_vectorization_report.txt

cleanall: clean
	-rm -fr $(TARGETS)
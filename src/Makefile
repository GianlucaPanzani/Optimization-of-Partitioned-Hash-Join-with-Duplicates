CXX            	= g++ -std=c++17
OPTFLAGS       	= -O3
AVXFLAGS       	= -mavx2
AUTOFLAGS      	= -fopt-info-vec-optimized -fopt-info-vec-missed
NOVECFLAGS     	= -fno-tree-vectorize
CXXFLAGS       += -Wall
INCLUDES       	= -I. -I./include
LIBS           	=

TARGETS_DATA 	= generate_datasets
TARGETS_CPU    	= plain_novec plain_vec
TARGETS_GPU		= avx2
TARGETS_ALL		= $(TARGETS_DATA) $(TARGETS_CPU) $(TARGETS_GPU)

LIB_SOURCES    	= lib/dataset.cpp lib/timing.cpp lib/checksum.cpp lib/metrics.cpp lib/partition.cpp lib/config.cpp
AVX_SOURCES 	= lib/partition_avx2.cpp
GCC_REPORT_PATH = results/gcc_vec_report.txt
RESULTS_FILES	= results/gcc_vec_report.txt results/checksum.txt results/throughput.txt results/timing.txt


.PHONY: all clean cleanall report

all: $(TARGETS_ALL)

generate_datasets: generate_datasets.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@ generate_datasets.cpp $(LIB_SOURCES) $(LIBS)

plain_novec: plain.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(NOVECFLAGS) -o $@ plain.cpp $(LIB_SOURCES) $(LIBS)

plain_vec: plain.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ plain.cpp $(LIB_SOURCES) $(LIBS) 2> $(GCC_REPORT_PATH)

avx2: avx2.cpp $(LIB_SOURCES) $(AVX_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AVXFLAGS) -o $@ avx2.cpp $(LIB_SOURCES) $(AVX_SOURCES) $(LIBS)

report: plain.cpp $(LIB_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o plain_vec plain.cpp $(LIB_SOURCES) $(LIBS) 2> $(GCC_REPORT_PATH)

launch-benchmark: LIBS += -pthread

clean:
	-rm -fr *.o *~

cleanall: clean
	-rm -fr $(TARGETS) $(GCC_REPORT_PATH) $(RESULTS_FILES)
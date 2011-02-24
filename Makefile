NUM = 1

CPP = g++
LLVMHOME = /home/modcarl/llvm-124522
LLVMCONFIG = $(LLVMHOME)/bin/llvm-config
CXXFLAGS = -g -fno-rtti \
		 `$(LLVMCONFIG) --cxxflags`

LDFLAGS= `$(LLVMCONFIG) --ldflags`
LIBS=  -lclangDriver -lclangFrontend -lclangIndex -lclangParse \
	   -lclangSema -lclangAnalysis -lclangAST -lclangLex -lclangBasic 

all:test
test:
	$(CPP) tut$(NUM).cpp $(CXXFLAGS) \
	-o tut1 $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
1:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
2:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
3:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
4:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
5:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
6:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`
7:
	$(CPP) tut$@.cpp $(CXXFLAGS) \
	-o tut$@ $(LIBS) `$(LLVMCONFIG) --ldflags --libs all`

CC = gcc
CXX = g++ -std=c++14

INCLUDES = -I/usr/local/include -I.
LIBRARIES = 

source_dir = .

CFLAGS = -Wall -ggdb -O3 $(INCLUDES)
CXXFLAGS = -Wall -ggdb -O3 $(INCLUDES)
LDFLAGS = $(LIBRARIES) -L/usr/local/lib

TARGET1 = ray_cast
cpp_files1 = ray_cast.cpp

objects1 = $(cpp_files1:.cpp=.o)

all: $(TARGET1)

$(TARGET1): $(objects1) 
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY : clean
clean:
	rm -f $(TARGET1) $(objects1)


# Author: Juergen Gall, BIWI, ETH Zurich
# Email:  gall@vision.ee.ethz.ch

# Change the path if required
INCLUDES = -I/usr/lib/nvidia/include -I./glh/inc -I. -I./shared/include 

LIBS = -lm -lGL -lGLU -lglut -lX11 -lXmu -lXi -lpng

# Change the path if required
LIBDIRS = -L/usr/lib/nvidia/lib 

# Compile Linux
OPT = -O3 -Wno-deprecated

CC=g++

.PHONY: all ShowMesh clean

OBJS = Show.o CTMeshLight.o CModelLight.o pbuffer.o CGrabber.o

clean:
		rm -f *.o *~ ShowMesh out
			
all:	ShowMesh
		echo all: make complete
%.o:%.cpp
	$(CC) -c $(INCLUDES) -DUNIX -DUSEGPU $+ $(OPT)
	
ShowMesh: $(OBJS) dynamicMesh.o
		$(CC) $(LIBDIRS) $(LIBS) -o $@ $+ $(OPT)




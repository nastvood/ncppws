CC=g++
CFLAGS=-c -Wall 
SOURCES=main.cpp src/NWSClient.h src/NWSClient.cpp src/NWSServer.h src/NWSServer.cpp src/NWSFrame.cpp src/NWSFrame.h src/wshelper.h src/wshelper.cpp src/wscrypto.h src/wscrypto.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ncppws
LDFLAGS=-lssl -lcrypto

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o src/*.o $(EXECUTABLE)

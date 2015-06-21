
CC=g++
CFLAGS=-c -std=c++11 -Wall -I.. -I../pugixml
LDFLAGS=
LIBS=-lboost_program_options

VPATH=../Utility:../XmlSys:../pugixml

UTILITY=FileListProcessor.h Iterable.h LineOutput.h
XMLSYS=XpathAgent.h XmlDoc.h AgentSet.h TargetMethods.h Mappers.h
HEADERS=$(UTILITY) $(XMLSYS)

SOURCES=XpMatch.cpp ../pugixml/pugixml.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=xpmatch

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

XpMatch.o: $(HEADERS) OutputMethods.h XpMatch.h

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
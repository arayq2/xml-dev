
CC=g++
CFLAGS=-c -std=c++11 -Wall -I.. -I../pugixml
LDFLAGS=
LIBS=-lboost_program_options

VPATH=../Utility:../DocSys:../XmlSys:../pugixml

UTILITY=FileListProcessor.h LineOutput.h
DOCSYS=AgentSet.h TargetMethods.h Mappers.h
XMLSYS=XpathAgent.h XmlDoc.h
HEADERS=$(UTILITY) $(DOCSYS) $(XMLSYS)

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

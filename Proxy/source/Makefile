

CXX = g++
CXXFLAGS = -I/usr/include/Sockets
LIBS = -lSockets -lssl -lpthread

libjson_Obj = \
	libjson/Source/internalJSONNode.o \
	libjson/Source/JSON_Base64.o \
	libjson/Source/JSONChildren.o \
	libjson/Source/JSONDebug.o \
	libjson/Source/JSONIterators.o \
	libjson/Source/JSONMemory.o \
	libjson/Source/JSONNode.o \
	libjson/Source/JSONNode_Mutex.o \
	libjson/Source/JSONStream.o \
	libjson/Source/JSONValidator.o \
	libjson/Source/JSONWorker.o \
	libjson/Source/JSONWriter.o \
	libjson/Source/libjson.o

Httpd_Obj = Httpd.o IrcClient.o IrcSession.o IrcSessionManager.o IrcSocket.o main.o ServerSocket.o Utils.o

all: http2irc


http2irc:	$(Httpd_Obj) $(libjson_Obj)
	$(CXX) -o http2irc $(Httpd_Obj) $(libjson_Obj) $(LIBS)


clean:
	- rm *.o
	- rm libjson/Source/*.o	
	- rm http2irc
	

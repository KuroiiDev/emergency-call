CC = g++
CFLAGS = -std=c++11 -pthread -Wall

SRCS = main.cpp \
       core/store.cpp \
       modules/call.cpp \
       modules/dispatch.cpp \
       modules/history.cpp \
       modules/search.cpp \
       modules/report.cpp \
       modules/help.cpp \
       utils/display.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = index

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

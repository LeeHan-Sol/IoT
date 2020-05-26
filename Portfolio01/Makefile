CC = gcc
CFLAGS = -W -Wall -g
TARGET = IoT
OBJECTS = DHT11.o TextLCD.o IoT.o
LDFLAGS = -lwiringPi -lwiringPiDev -lpthread 

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm DHT11.o TextLCD.o IoT.o IoT

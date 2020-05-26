CC = gcc
CFLAGS = -W -Wall
TARGET = IoT
OBJECTS = DHT11.o TextLCD.o IoT.o
LDFLAGS = -lwiringPi -lwiringPiDev 

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm DHT11.o TextLCD.o IoT.o IoT

CC = gcc
CFLAGS = -W -Wall -g -D_REENTRANT
TARGET = IoT
OBJECTS = Error.o Node.o List.o DHT11.o TextLCD.o LED.o Button.o IoT.o Thread.o
LDFLAGS = -lwiringPi -lwiringPiDev -lpthread 

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm Error.o Node.o List.o DHT11.o TextLCD.o LED.o Button.o IoT.o Thread.o IoT

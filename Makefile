BINARY_NAME=fbd
FLAGS=-std=c++11 `pkg-config --cflags --libs libdrm`
ALL_FILES=Screen.cpp FbScreen.cpp LinuxFbScreen.cpp LinuxFbDrmDevice.cpp LinuxFbDrmScreen.cpp DrawEngine.cpp Image.cpp Painter.cpp Rect.cpp Size.cpp main.cpp

default:
	g++ -o $(BINARY_NAME) $(ALL_FILES) $(FLAGS)

debug:
	g++ -g -o $(BINARY_NAME) $(ALL_FILES) $(FLAGS)

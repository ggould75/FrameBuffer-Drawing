BINARY_NAME=fbd
FLAGS=`pkg-config --cflags --libs libdrm`
ALL_FILES=Screen.cpp FbScreen.cpp LinuxFbScreen.cpp LinuxFbDrmDevice.cpp LinuxFbDrmScreen.cpp GraphicsContext.cpp GraphicsContext_p.cpp Image.cpp Painter.cpp Rect.cpp Size.cpp main.cpp

default:
	g++ -o $(BINARY_NAME) $(ALL_FILES) $(FLAGS)

debug:
	g++ -g -o $(BINARY_NAME) $(ALL_FILES) $(FLAGS)

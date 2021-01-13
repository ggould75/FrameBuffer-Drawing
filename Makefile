FLAGS=`pkg-config --cflags --libs libdrm`

default:
	g++ -o fbd Screen.cpp FbScreen.cpp LinuxFbScreen.cpp LinuxFbDrmDevice.cpp LinuxFbDrmScreen.cpp GraphicsContext.cpp GraphicsContext_p.cpp Image.cpp main.cpp $(FLAGS)

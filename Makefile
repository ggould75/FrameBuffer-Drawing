FLAGS=`pkg-config --cflags --libs libdrm`

all:
	g++ -o fbd -g LinuxFrameBuffer.cpp GraphicsContext.cpp GraphicsContext_p.cpp DrmDevice.cpp main.cpp $(FLAGS)

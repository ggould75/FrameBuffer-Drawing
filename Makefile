FLAGS=`pkg-config --cflags --libs libdrm`

default:
	g++ -o fbd LinuxFrameBuffer.cpp DrmDevice.cpp GraphicsContext.cpp GraphicsContext_p.cpp main.cpp $(FLAGS)

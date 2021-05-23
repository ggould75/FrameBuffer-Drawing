FLAGS=`pkg-config --cflags --libs libdrm`

default:
	g++ -o fbd LinuxFrameBuffer.cpp GraphicsContext.cpp GraphicsContext_p.cpp DrmDevice.cpp main.cpp $(FLAGS)

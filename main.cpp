#include "LinuxFbScreen.h"
#include "LinuxFbDrmScreen.h"
#include "common.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

void usageError(const char *programName)
{
    fflush(stdout);
    fprintf(stderr, "Usage: %s [-fb[ ][number]]\n", programName);
    fprintf(stderr, "If no options are provided, DRM will be used.\n");
    exit(EXIT_FAILURE);
}

bool parseInt(const char *str, long *value)
{
    char *endPtr;
    *value = strtol(str, &endPtr, 10);
    
    return !( (*value == 0 && errno == EINVAL) || 
               errno == ERANGE || 
               *endPtr != '\0' );
}

int main(int argc, const char *argv[])
{
    Screen *screen;
    
    if (argc >= 2) {
        short int frameBufferNumber = 0;
        
        if (strncmp(argv[1], "-fb", 3) == 0) {
            const char *candidateFbStr = argc > 2 ? argv[2] : &argv[1][3];
            long candidateFb;
            if (*candidateFbStr != '\0') {
                if (parseInt(candidateFbStr, &candidateFb)) {
                    frameBufferNumber = candidateFb;
                } else {
                    usageError(argv[0]);
                }
            }
        } else {
            usageError(argv[0]);
        }
        
        screen = new LinuxFbScreen(frameBufferNumber);
    } else {
        screen = new LinuxFbDrmScreen();
    }
    
    screen->initialize();
    
    LinuxFbScreen *fbScreen = dynamic_cast<LinuxFbScreen *>(screen);
    if (fbScreen) {
        fbScreen->clearScreen();
    }
    
/**
    GraphicsContext gc = GraphicsContext();
    gc.setup();
    
    gc.drawPixel(100, 200, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 201, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 202, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 203, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 204, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 205, 0xFF, 0x00, 0x00);

    // vertical lines
    gc.drawLine(900, 400, 900, 600);
    gc.drawLine(800, 600, 800, 400);
    // horizontal lines
    gc.drawLine(1000, 400, 1400, 400);
    // dots
    gc.drawLine(500, 500, 500, 500);
    gc.drawLine(900, 200, 900, 200);
    
    // rectangle
    gc.drawLine(800, 300, 1800, 300);
    gc.drawLine(1800, 300, 1800, 1000);
    gc.drawLine(1800, 1000, 800, 1000);
    gc.drawLine(800, 1000, 800, 300);
    
    gc.drawCircle(500, 200, 100);
    
    sleep(2);
    
    for (int i = 0; i < 500; i++) {
        gc.fillRect(2000, 300, 300, 150);
        gc.clear();
    }
*/
    return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "fbglTypes.h"

#define WHICH_FB "fb0"

int initScreen(struct Screen* screen) {
    struct fb_var_screeninfo fbInfoV;
    struct fb_fix_screeninfo fbInfoF;

    screen->fbFile = open("/dev/" WHICH_FB, O_RDWR);
    if (screen->fbFile == -1) {
        perror("Could not access framebuffer at /dev/" WHICH_FB "!\n");
        return 1;
    }
    printf("Framebuffer accessed: " WHICH_FB "\n");

    if (ioctl(screen->fbFile, FBIOGET_FSCREENINFO, &fbInfoF) == -1) {
        perror("Could not access fixed framebuffer information!\n");
        return 2;
    }
    if (ioctl(screen->fbFile, FBIOGET_VSCREENINFO, &fbInfoV) == -1) {
        perror("Could not access variable framebuffer information!\n");
        return 3;
    }
    screen->width = fbInfoV.xres;
    screen->height = fbInfoV.yres;
    screen->BpPixel = fbInfoV.bits_per_pixel / 8;
    screen->BpLine = fbInfoF.line_length;
    screen->BpFrame =  screen->width * screen->height * screen->BpPixel;
    printf("Screen: %dx%d, %dbpp\n", screen->width, screen->height, fbInfoV.bits_per_pixel);

    screen->fb = (uint8_t*)mmap(0, screen->BpFrame, PROT_READ | PROT_WRITE, MAP_SHARED, screen->fbFile, 0);
    if ((int)screen->fb == -1) {
        perror("Could not map framebuffer to memory!\n");
        return 4;
    }
    printf("Framebuffer mapped to memory.\n");

    return 0;  
}

void destroySubScreen(struct SubScreen* subscr) {
	if (subscr != NULL) {
		if (subscr->sibling) {
			destroySubScreen(subscr->sibling);
		}
		if (subscr->child) {
			destroySubScreen(subscr->child);
		}
	}
}

int destroyScreen(struct Screen* screen) {
	destroySubScreen(screen->child);
    munmap(screen->fb, screen->BpFrame);
    close(screen->fbFile);
    return 0;
}

int main()
{
    struct Screen screen;
    int result = initScreen(&screen);
    if (result == 0) { // If init was successful
        int x, y, pixOffset;
        for (y = 100; y < 300; y++) {
            for (x = 100; x < 300; x++) {

                pixOffset = x * screen.BpPixel +
                           y * screen.BpLine;

                *(screen.fb + pixOffset) = 15+(x-100)/2;
                *(screen.fb + pixOffset + 1) = 200-(y-100)/5;
                *(screen.fb + pixOffset + 2) = 100;
                *(screen.fb + pixOffset + 3) = 0;
            }
        }
    }

    destroyScreen(&screen);
    return result;
}




















// int main()
// {
//     int fbFile = 0;
//     struct fb_var_screeninfo fbInfoV;
//     struct fb_fix_screeninfo fbInfoF;
//     long int fbSizeBytes = 0;
//     char* fbp = 0;
//     int x = 0, y = 0;
//     long int location = 0;

//     // Open the file for reading and writing
//     fbFile = open("/dev/fb0", O_RDWR);
//     if (fbFile == -1) {
//         perror("Error: cannot open framebuffer device");
//         exit(1);
//     }
//     printf("The framebuffer device was opened successfully.\n");

//     // Get fixed screen information
//     if (ioctl(fbFile, FBIOGET_FSCREENINFO, &fbInfoF) == -1) {
//         perror("Error reading fixed information");
//         exit(2);
//     }

//     // Get variable screen information
//     if (ioctl(fbFile, FBIOGET_VSCREENINFO, &fbInfoV) == -1) {
//         perror("Error reading variable information");
//         exit(3);
//     }

//     printf("%dx%d, %dbpp\n", fbInfoV.xres, fbInfoV.yres, fbInfoV.bits_per_pixel);

//     // Figure out the size of the screen in bytes
//     fbSizeBytes = fbInfoV.xres * fbInfoV.yres * fbInfoV.bits_per_pixel / 8;

//     // Map the device to memory
//     fbp = (char *)mmap(0, fbSizeBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbFile, 0);
//     if ((int)fbp == -1) {
//         perror("Error: failed to map framebuffer device to memory");
//         exit(4);
//     }
//     printf("The framebuffer device was mapped to memory successfully.\n");

//     x = 100; y = 100;       // Where we are going to put the pixel

//     // Figure out where in memory to put the pixel
//     for (y = 100; y < 300; y++)
//         for (x = 100; x < 300; x++) {

//             location = (x+fbInfoV.xoffset) * (fbInfoV.bits_per_pixel/8) +
//                        (y+fbInfoV.yoffset) * fbInfoF.line_length;

//             if (fbInfoV.bits_per_pixel == 32) {
//                 *(fbp + location) = 15+(x-100)/2;
//                 *(fbp + location + 1) = 200-(y-100)/5;
//                 *(fbp + location + 2) = 100;
//                 *(fbp + location + 3) = 0;
//         //location += 4;
//             } else  { //assume 16bpp
//                 int b = 10;
//                 int g = (x-100)/6;     // A little green
//                 int r = 31-(y-100)/16;    // A lot of red
//                 unsigned short int t = r<<11 | g << 5 | b;
//                 *((unsigned short int*)(fbp + location)) = t;
//             }

//         }
//     munmap(fbp, fbSizeBytes);
//     close(fbFile);
//     return 0;
// }
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "fbgl.h"

#define WHICH_FB "fb0"

static int fbgl_addSibling(fbgl_SubScreen* first, fbgl_SubScreen* second) {
	if (first->sibling) {
		return fbgl_addSibling(first->sibling, second);
	} else {
		first->sibling = second;
		return 0;
	}
}

static void fbgl_initObjCollection(fbgl_ObjCollection* objs) {
	objs->os2 = NULL;
	objs->ov2 = NULL;
	objs->ot2 = NULL;
	objs->ls2 = NULL;
	objs->lv2 = NULL;
	objs->lt2 = NULL;
}

void fbgl_initSubScr(fbgl_SubScreen* subscr) {
	subscr->posX = -0.9f;
	subscr->posY = -0.9f;
	subscr->width = 1.8f;
	subscr->height = 1.8f;
	subscr->r = 64;
	subscr->g = 255;
	subscr->b = 64;
	subscr->a = 255;
	subscr->style = LINES;
	fbgl_initObjCollection(&subscr->objs);
	subscr->child = NULL;
	subscr->sibling = NULL;
}

void fbgl_destroySubScr(fbgl_SubScreen* subscr) {
	if (subscr->sibling) {
		fbgl_destroySubScr(subscr->sibling);
		subscr->sibling = NULL;
	}
	if (subscr->child) {
		fbgl_destroySubScr(subscr->child);
		subscr->child = NULL;
	}
}

void fbgl_destroyScreen(fbgl_Screen* screen) {
	if (screen->child) {
		fbgl_destroySubScr(screen->child);
		screen->child = NULL;
	}
    munmap(screen->fb, screen->BpScreen);
    close(screen->fbFile);
}

int fbgl_initScreen(fbgl_Screen* screen) {
	screen->fb = NULL;
	screen->child = NULL;

    struct fb_var_screeninfo fbInfoV;
    struct fb_fix_screeninfo fbInfoF;

    screen->fbFile = open("/dev/" WHICH_FB, O_RDWR);
    if (screen->fbFile == -1) {
        perror("Could not access framebuffer at /dev/" WHICH_FB "!\n");
        return 1;
    }
    if (ioctl(screen->fbFile, FBIOGET_FSCREENINFO, &fbInfoF) == -1) {
        perror("Could not access fixed framebuffer information!\n");
        return 2;
    }
    if (ioctl(screen->fbFile, FBIOGET_VSCREENINFO, &fbInfoV) == -1) {
        perror("Could not access variable framebuffer information!\n");
        return 3;
    }
    screen->width = fbInfoV.xres;
    screen->normFactorX = 2.f / (float)screen->width;
    screen->deNormFactorX = (float)screen->width / 2.f;
    screen->height = fbInfoV.yres;
    screen->normFactorY = 2.f / (float)screen->height;
    screen->deNormFactorY = (float)screen->height / 2.f;
    screen->BpPixel = fbInfoV.bits_per_pixel / 8;
    screen->BpLine = fbInfoF.line_length;
    screen->BpScreen =  screen->width * screen->height * screen->BpPixel;

    screen->fb = (uint8_t*)mmap(0, screen->BpScreen, PROT_READ | PROT_WRITE, MAP_SHARED, screen->fbFile, 0);
    if ((intptr_t)screen->fb == -1) {
        perror("Could not map framebuffer to memory!\n");
        return 4;
    }

    return 0;  
}

int fbgl_makeSubScrMajor(fbgl_Screen* parentScreen, fbgl_SubScreen* subscr) {
	if (parentScreen->child) {
		return fbgl_addSibling(parentScreen->child, subscr);
	} else {
		parentScreen->child = subscr;
		return 0;
	}
}

int fbgl_makeSubScrMinor(fbgl_SubScreen* parentSubscr, fbgl_SubScreen* subscr) {
	// keep list of subscrs that have been used as parameters, then if second parameter is in that list, error (loop)
	// OR use Floyd's algorithm on each sibling (at end of function)

	if (parentSubscr->child) {
		return fbgl_addSibling(parentSubscr->child, subscr);
	} else {
		parentSubscr->child = subscr;
		return 0;
	}
}

// Coolest function ever.
static inline int fbgl_sign(int val) {
    return (0 < val) - (val < 0);
    // -1 if neg, 0 if 0, +1 if pos
}

static inline void fbgl_drawPixel(fbgl_Screen* screen, int x, int y,
								  int r, int g, int b, int a) {
	uint32_t pixOffset = x * screen->BpPixel + y * screen->BpLine;	
	if (pixOffset <= screen->BpScreen) {
		uint8_t* memLoc = screen->fb + pixOffset;
		*memLoc   = b;
	    *++memLoc = g;
	    *++memLoc = r;
	    *++memLoc = a;
	}
}

static void fbgl_drawLineSolid(fbgl_Screen* screen, int x0, int y0,
							   int x1, int y1, int r, int g, int b, int a){
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int swap = 0;
	if (dy > dx) {
		int temp = dx;
		dx = dy;
		dy = temp;
		swap = 1;
	}
	int s1 = fbgl_sign(x1 - x0);
	int s2 = fbgl_sign(y1 - y0);
	int D = 2 * dy - dx;
	int x = x0;
	int y = y0;
	int i;
	for (i = 0; i < dx; i++)
	{
		fbgl_drawPixel(screen, x, y, r, g, b, a);
		while (D >= 0) {
			D = D - 2 * dx;
			if (swap) {
				x += s1;
			} else {
				y += s2;
			}
		}
		D = D + 2 * dy;
		if (swap) {
			y += s2;
		} else {
			x += s1;
		}
	}
}

static void fbgl_drawSubScrXBox(fbgl_Screen* screen, fbgl_SubScreen* s,
						        int minX, int maxX, int minY, int maxY) {
	fbgl_drawLineSolid(screen, minX, minY, minX, maxY, s->r, s->g, s->b, s->a);
	fbgl_drawLineSolid(screen, minX, maxY, maxX, maxY, s->r, s->g, s->b, s->a);
	fbgl_drawLineSolid(screen, maxX, maxY, maxX, minY, s->r, s->g, s->b, s->a);
	fbgl_drawLineSolid(screen, maxX, minY, minX, minY, s->r, s->g, s->b, s->a);
	fbgl_drawLineSolid(screen, minX, minY, maxX, maxY, s->r, s->g, s->b, s->a);
	fbgl_drawLineSolid(screen, maxX, minY, minX, maxY, s->r, s->g, s->b, s->a);
}

static void fbgl_drawSubScrFill(fbgl_Screen* screen, fbgl_SubScreen* s,
						  	    int minX, int maxX, int minY, int maxY) {
	int x, y;
	for (y = minY; y > maxY; --y) {	// y is upside down
        for (x = minX; x < maxX; ++x) {
        	fbgl_drawPixel(screen, x, y, s->r, s->g, s->b, s->a);
        }
    }
}

static void fbgl_drawSubScr(fbgl_Screen* screen, fbgl_SubScreen* s) {
	int minX = (int)((s->posX + 1.f) * screen->deNormFactorX);
	int maxX = (int)((s->posX + s->width + 1.f) * screen->deNormFactorX);
	int minY = (int)((1.f - s->posY) * screen->deNormFactorY);
	int maxY = (int)((1.f - (s->posY + s->height)) * screen->deNormFactorY);

	if (s->style == LINES) {
		fbgl_drawSubScrXBox(screen, s, minX, maxX, minY, maxY);
	} else if (s->style == SOLID) {
		fbgl_drawSubScrFill(screen, s, minX, maxX, minY, maxY);
	} else {
		printf ("Only subscreen fill styles LINES and SOLID are currently supported.\n");
	}

	if (s->child) {
		fbgl_drawSubScr(screen, s->child);
	}
	if (s->sibling) {
		fbgl_drawSubScr(screen, s->sibling);
	}
}

void fbgl_draw(fbgl_Screen* screen) {

	if (screen->child) {
		fbgl_drawSubScr(screen, screen->child);
	}
}

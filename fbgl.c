#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "fbgl.h"
#include "fbglMath.h"

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

static void fbgl_subscrUpdateMat(fbgl_SubScreen* subscr) {
	fbgl_Mat4 transMat, scaleMat;
	fbgl_mat4Translate(&transMat, subscr->posX, subscr->posY, 0.f);
    fbgl_mat4Scale(&scaleMat, subscr->width * 0.5, subscr->height * 0.5, 1.f);
    fbgl_mat4xMat4(&subscr->screenMat, &transMat, &scaleMat);
}

void fbgl_initSubScr(fbgl_SubScreen* subscr) {
	subscr->posX = 0.f;
	subscr->posY = 0.f;
	subscr->width = 1.8f;
	subscr->height = 1.8f;
	subscr->r = 0;
	subscr->g = 94;
	subscr->b = 0;
	subscr->a = 255;
	subscr->style = XBOX;
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
    screen->height = fbInfoV.yres;
    screen->BpPixel = fbInfoV.bits_per_pixel / 8;
    screen->BpLine = fbInfoF.line_length;
    screen->BpScreen = screen->width * screen->height * screen->BpPixel;
    
    fbgl_Mat4 transMat, scaleMat;
    fbgl_mat4Translate(&transMat, 1.f, -1.f, 0.f);
    fbgl_mat4Scale(&scaleMat,
    	(float)screen->width / 2.f, (float)screen->height / -2.f, 1.f);
    fbgl_mat4xMat4(&screen->deviceMat.mat, &scaleMat, &transMat);
    screen->stackTop = &screen->deviceMat;

    unsigned int n;
    for (n = 0; n < 4; ++n) {
    	screen->corners[n].v[0] = n % 2 ? 1.f : -1.f;
    	screen->corners[n].v[1] = n / 2 ? -1.f : 1.f;
    	screen->corners[n].v[2] = 0.f;
    	screen->corners[n].v[3] = 1.f;
    }

    screen->fb = (uint8_t*)mmap(0, screen->BpScreen, PROT_READ | PROT_WRITE, MAP_SHARED, screen->fbFile, 0);
    if ((intptr_t)screen->fb == -1) {
        perror("Could not map framebuffer to memory!\n");
        return 4;
    }

    return 0;  
}

int fbgl_makeSubScrMajor(fbgl_Screen* parentScreen, fbgl_SubScreen* subscr) {
	fbgl_subscrUpdateMat(subscr);
	if (parentScreen->child) {
		return fbgl_addSibling(parentScreen->child, subscr);
	} else {
		parentScreen->child = subscr;
		return 0;
	}
}

int fbgl_makeSubScrMinor(fbgl_SubScreen* parentSubscr, fbgl_SubScreen* subscr) {
	// keep list of subscrs that have been used as parameters, then if second
	// parameter is in that list, error (loop) OR use Floyd's algorithm on
	// each sibling (at end of function)

	fbgl_subscrUpdateMat(subscr);
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
}

// inline?
static void fbgl_drawPixel(	fbgl_Screen* screen, int x, int y,
				int r, int g, int b, int a) {
	uint32_t pixOffset = x * screen->BpPixel + y * screen->BpLine;	
	if (pixOffset < screen->BpScreen) {
		uint8_t* memLoc = screen->fb + pixOffset;
		*memLoc   = b;
	 	*++memLoc = g;
	 	*++memLoc = r;
	 	*++memLoc = a;
	}
}

// Bresenham's algorithm
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
	for (y = minY; y < maxY; ++y) {	// y is upside down
        for (x = minX; x < maxX; ++x) {
        	fbgl_drawPixel(screen, x, y, s->r, s->g, s->b, s->a);
        }
    }
}

static void fbgl_drawOs2(fbgl_Screen* screen, fbgl_ObjectSolid2d* obj) {
	if (obj->next) {
		fbgl_drawOs2(screen, obj->next);
	}
}
static void fbgl_drawOv2(fbgl_Screen* screen, fbgl_ObjectVaried2d* obj) {
	if (obj->next) {
		fbgl_drawOv2(screen, obj->next);
	}
}
static void fbgl_drawOt2(fbgl_Screen* screen, fbgl_ObjectTexed2d* obj) {
	if (obj->next) {
		fbgl_drawOt2(screen, obj->next);
	}
}
static void fbgl_drawLs2(fbgl_Screen* screen, fbgl_LineSolid2d* obj) {
	if (obj->next) {
		fbgl_drawLs2(screen, obj->next);
	}
}
static void fbgl_drawLv2(fbgl_Screen* screen, fbgl_LineVaried2d* obj) {
	if (obj->next) {
		fbgl_drawLv2(screen, obj->next);
	}
}
static void fbgl_drawLt2(fbgl_Screen* screen, fbgl_LineTexed2d* obj) {
	if (obj->next) {
		fbgl_drawLt2(screen, obj->next);
	}
}

static void fbgl_pushMatrix(fbgl_Screen* screen, fbgl_Mat4StackNode* node,
							fbgl_Mat4* mat) {
	fbgl_mat4xMat4(&node->mat, &screen->stackTop->mat, mat);
	node->last = screen->stackTop;
	screen->stackTop = node;
}

static void fbgl_popMatrix(fbgl_Screen* screen) {
	screen->stackTop = screen->stackTop->last;
}

static void fbgl_drawSubScr(fbgl_Screen* screen, fbgl_SubScreen* s) {
	fbgl_Mat4StackNode node;
	fbgl_pushMatrix(screen, &node, &s->screenMat);
	
	if (s->style != NONE) {
		fbgl_Vec4 vTL, vBR;
		fbgl_mat4xVec4(&vTL, &screen->stackTop->mat, &screen->corners[0]);
		fbgl_mat4xVec4(&vBR, &screen->stackTop->mat, &screen->corners[3]);
		int minX = (int)vTL.v[0];
		int maxX = (int)vBR.v[0];
		int minY = (int)vTL.v[1];
		int maxY = (int)vBR.v[1];

		switch(s->style) {
			case XBOX:
				fbgl_drawSubScrXBox(screen, s, minX, maxX, minY, maxY);
				break;
			case SOLID:
				fbgl_drawSubScrFill(screen, s, minX, maxX, minY, maxY);
				break;
			default:
				break;
		}		
	}

	if (s->objs.os2 != NULL) fbgl_drawOs2(screen, s->objs.os2);
	if (s->objs.ov2 != NULL) fbgl_drawOv2(screen, s->objs.ov2);
	if (s->objs.ot2 != NULL) fbgl_drawOt2(screen, s->objs.ot2);
	if (s->objs.ls2 != NULL) fbgl_drawLs2(screen, s->objs.ls2);
	if (s->objs.lv2 != NULL) fbgl_drawLv2(screen, s->objs.lv2);
	if (s->objs.lt2 != NULL) fbgl_drawLt2(screen, s->objs.lt2);

	if (s->child) {
		fbgl_drawSubScr(screen, s->child);
	}
	if (s->sibling) {
		fbgl_drawSubScr(screen, s->sibling);
	}

	fbgl_popMatrix(screen);
}

void fbgl_draw(fbgl_Screen* screen) {

	if (screen->child) {
		fbgl_drawSubScr(screen, screen->child);
	}
}

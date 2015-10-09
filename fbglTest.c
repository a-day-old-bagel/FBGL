#define FBGL_SHORT_NAMES // This is like a namespace
#include "fbgl.h"

int main() {
	Screen screen;
    int initResult = initScreen(&screen);

	if (initResult == 0){
    	SubScreen subscr0;
    	initSubScr(&subscr0);
    	makeSubScrMajor(&screen, &subscr0);

    	SubScreen subscr1;
    	initSubScr(&subscr1);
    	subscr1.posX = 0.f;
    	subscr1.posY = 0.f;
    	subscr1.width = 0.7f;
    	subscr1.height = 0.8f;
    	subscr1.r = 255;
    	subscr1.g = 128;
    	subscr1.b = 64;
    	subscr1.a = 255;
    	makeSubScrMinor(&subscr0, &subscr1);

    	SubScreen subscr2;
    	initSubScr(&subscr2);
    	subscr2.posX = 0.3f;
    	subscr2.posY = 0.85f;
    	subscr2.width = 1.25f;
    	subscr2.height = 1.7f;
    	subscr2.r = 32;
    	subscr2.g = 128;
    	subscr2.b = 255;
    	subscr2.a = 255;
    	//subscr2.style = SOLID;
    	makeSubScrMajor(&screen, &subscr2);

    	SubScreen subscr3;
    	initSubScr(&subscr3);
    	subscr3.posX = -0.4f;
    	subscr3.posY = -0.7f;
    	subscr3.width = 0.5f;
    	subscr3.height = 1.3f;
    	subscr3.r = 128;
    	subscr3.g = 128;
    	subscr3.b = 255;
    	subscr3.a = 255;
    	//subscr3.style = SOLID;
    	makeSubScrMinor(&subscr2, &subscr3);
    
    	draw(&screen);
    }

    destroyScreen(&screen);
    return initResult;
}
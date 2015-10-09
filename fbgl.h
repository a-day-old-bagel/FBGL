#ifndef FBGL_HEADER
#define FBGL_HEADER

#include <stdint.h>

/*******************************************************************************
* FBGL DATA ACCESS DEFINES (LIKE ENUMERATORS)
*******************************************************************************/
#define _B 0
#define _G 1
#define _R 2
#define _A 3

#define _X  0
#define _Y  1
#define _Z  2
#define _W2 2
#define _W3 3

typedef enum {NONE, LINES, SOLID, COLORED, TEXTURED} FillStyle;

/*******************************************************************************
* FBGL TYPES
*******************************************************************************/
typedef struct fbgl_ObjectSolid2d 	fbgl_ObjectSolid2d;
typedef struct fbgl_ObjectVaried2d 	fbgl_ObjectVaried2d;
typedef struct fbgl_ObjectTexed2d 	fbgl_ObjectTexed2d;
typedef struct fbgl_LineSolid2d 	fbgl_LineSolid2d;
typedef struct fbgl_LineVaried2d 	fbgl_LineVaried2d;
typedef struct fbgl_LineTexed2d 	fbgl_LineTexed2d;
typedef struct fbgl_ObjCollection 	fbgl_ObjCollection;
typedef struct fbgl_SubScreen 		fbgl_SubScreen;
typedef struct fbgl_Screen 			fbgl_Screen;
typedef struct fbgl_Vec2			fbgl_Vec2;
typedef struct fbgl_Vec3  			fbgl_Vec3;
typedef struct fbgl_Vec4  			fbgl_Vec4;
typedef struct fbgl_Color 			fbgl_Color;
typedef struct fbgl_Texture			fbgl_Texture;
typedef struct fbgl_TexCoord		fbgl_TexCoord;
typedef struct fbgl_Mat3			fbgl_Mat3;
typedef struct fbgl_Mat4			fbgl_Mat4;
typedef struct fbgl_Mat4StackNode   fbgl_Mat4StackNode;
#ifdef FBGL_SHORT_NAMES
typedef struct fbgl_ObjectSolid2d 	ObjectSolid2d;
typedef struct fbgl_ObjectVaried2d 	ObjectVaried2d;
typedef struct fbgl_ObjectTexed2d 	ObjectTexed2d;
typedef struct fbgl_LineSolid2d 	LineSolid2d;
typedef struct fbgl_LineVaried2d 	LineVaried2d;
typedef struct fbgl_LineTexed2d 	LineTexed2d;
typedef struct fbgl_ObjCollection 	ObjCollection;
typedef struct fbgl_SubScreen 		SubScreen;
typedef struct fbgl_Screen 			Screen;
typedef struct fbgl_Vec2			Vec2;
typedef struct fbgl_Vec3  			Vec3;
typedef struct fbgl_Vec4  			Vec4;
typedef struct fbgl_Color 			Color;
typedef struct fbgl_Texture			Texture;
typedef struct fbgl_TexCoord		TexCoord;
typedef struct fbgl_Mat3			Mat3;
typedef struct fbgl_Mat4			Mat4;
typedef struct fbgl_Mat4StackNode   Mat4StackNode;
#endif

/*******************************************************************************
* FBGL TYPE DEFINITIONS
*******************************************************************************/
struct fbgl_Vec2 {
	float v[2];
};
struct fbgl_Vec3 {
	float v[3];
};
struct fbgl_Vec4 {
	float v[4];
};
struct fbgl_Color {
	uint8_t c[4];
};
struct fbgl_Texture {
	uint8_t* image;
	int width;
	int height;
};
struct fbgl_TexCoord {
	float u;
	float v;
};
struct fbgl_Mat3 {
	float m[3][3];
};
struct fbgl_Mat4 {
	float m[4][4];
};
struct fbgl_Mat4StackNode {
	fbgl_Mat4 mat;
	fbgl_Mat4StackNode* last;
};
struct fbgl_ObjectSolid2d {
	fbgl_Vec2* vertices;
	fbgl_Color fbgl_Color;
	fbgl_ObjectSolid2d* next;
};
struct fbgl_ObjectVaried2d {
	fbgl_Vec2* vertices;
	fbgl_Color* fbgl_Colors;
	fbgl_ObjectVaried2d* next;
};
struct fbgl_ObjectTexed2d {
	fbgl_Vec2* vertices;
	fbgl_TexCoord* uvs;
	fbgl_Texture tex;
	fbgl_ObjectTexed2d* next;
};
struct fbgl_LineSolid2d {
	fbgl_ObjectSolid2d obj;
	int thickness;
	fbgl_LineSolid2d* next;
};
struct fbgl_LineVaried2d {
	fbgl_ObjectVaried2d obj;
	int thickness;
	fbgl_LineVaried2d* next;
};
struct fbgl_LineTexed2d {
	fbgl_ObjectTexed2d obj;
	int thickness;
	fbgl_LineTexed2d* next;
};
struct fbgl_ObjCollection {
	fbgl_ObjectSolid2d* os2;
	fbgl_ObjectVaried2d* ov2;
	fbgl_ObjectTexed2d* ot2;
	fbgl_LineSolid2d* ls2;
	fbgl_LineVaried2d* lv2;
	fbgl_LineTexed2d* lt2;
};
struct fbgl_SubScreen {
    float posX, posY;
    float width, height;
    int r, g, b, a;
    FillStyle style;
    fbgl_ObjCollection objs;
    fbgl_SubScreen* child;
    fbgl_SubScreen* sibling;
    fbgl_Mat4 screenMat;
};
struct fbgl_Screen {
    uint8_t* fb;
    int fbFile;
    uint16_t width, height;
    float normFactorX, deNormFactorX;
    float normFactorY, deNormFactorY;
    uint8_t BpPixel;  // Bytes per pixel
    uint16_t BpLine;  // Bytes per line
    uint32_t BpScreen; // Bytes per frame
    fbgl_Vec4 corners[4];
    fbgl_Mat4StackNode deviceMat;
    fbgl_Mat4StackNode* stackTop;
    fbgl_SubScreen* child;
};

/*******************************************************************************
* FBGL FUNCTIONS
*******************************************************************************/
void fbgl_destroySubScr(fbgl_SubScreen* subscr);
void fbgl_destroyScreen(fbgl_Screen* screen);
void fbgl_initSubScr(fbgl_SubScreen* subScr);
int fbgl_initScreen(fbgl_Screen* screen);
int fbgl_makeSubScrMajor(fbgl_Screen* parentScreen, fbgl_SubScreen* subscr);
int fbgl_makeSubScrMinor(fbgl_SubScreen* parentSubscr, fbgl_SubScreen* subscr);
void fbgl_draw(fbgl_Screen* screen);

/*******************************************************************************
* FBGL FUNCTION NAME SHORTENING
*******************************************************************************/
#ifdef FBGL_SHORT_NAMES
#define destroySubScr(...)	 	fbgl_destroySubScr(__VA_ARGS__)
#define destroyScreen(...)		fbgl_destroyScreen(__VA_ARGS__)
#define initSubScr(...)			fbgl_initSubScr(__VA_ARGS__)
#define initScreen(...)			fbgl_initScreen(__VA_ARGS__)
#define makeSubScrMajor(...)	fbgl_makeSubScrMajor(__VA_ARGS__)
#define makeSubScrMinor(...)	fbgl_makeSubScrMinor(__VA_ARGS__)
#define draw(...)				fbgl_draw(__VA_ARGS__)
#endif

#endif
#include <stdint.h>

#define _B 0
#define _G 1
#define _R 2
#define _A 3

#define _X  0
#define _Y  1
#define _Z  2
#define _W2 2
#define _W3 3

struct Vertex2d {
	float pos[3];
};

struct Vertex3d {
	float pos[4];
};

struct Color {
	float val[4];
};

struct Texture {
	uint8_t* image;
	int width;
	int height;
};

struct TexCoord {
	float u;
	float v;
};

struct Mat3 {
	float val[9];
};

struct Mat4 {
	float val[16];
};

struct ObjectSolid2d {
	struct Vertex2d* vertices;
	struct Color color;
	struct ObjectSolid2d* next;
};

struct ObjectVaried2d {
	struct Vertex2d* vertices;
	struct Color* colors;
	struct ObjectVaried2d* next;
};

struct ObjectTexed2d {
	struct Vertex2d* vertices;
	struct TexCoord* uvs;
	struct Texture tex;
	struct ObjectTexed2d* next;
};

struct LineSolid2d {
	struct ObjectSolid2d obj;
	int thickness;
	struct LineSolid2d* next;
};

struct LineVaried2d {
	struct ObjectVaried2d obj;
	int thickness;
	struct LineVaried2d* next;
};

struct LineTexed2d {
	struct ObjectTexed2d obj;
	int thickness;
	struct LineTexed2d* next;
};

struct ObjCollection {
	struct ObjectSolid2d* os2;	// all start null
	struct ObjectVaried2d* ov2;
	struct ObjectTexed2d* ot2;
	struct LineSolid2d* ls2;
	struct LineVaried2d* lv2;
	struct LineTexed2d* lt2;
};

struct SubScreen {
    uint16_t posX;
    uint16_t posY;
    uint16_t width;
    uint16_t height;
    struct ObjCollection objs;
    struct SubScreen* child;	// starts null
    struct SubScreen* sibling;	// starts null
};

struct Screen {
    uint8_t* fb;
    int fbFile;
    uint16_t width;
    uint16_t height;
    uint8_t BpPixel;  // Bytes per pixel
    uint16_t BpLine;  // Bytes per line
    uint32_t BpFrame; // Bytes per frame
    struct SubScreen* child;	// starts null
};
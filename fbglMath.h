#ifndef FBGL_HEADER_MATH
#define FBGL_HEADER_MATH

#include "fbgl.h"

// For these, pass in a 4x4 matrix rhs to modify.
void fbgl_mat4Ident(fbgl_Mat4* rhs);
void fbgl_mat4Scale(fbgl_Mat4* rhs, float x, float y, float z);
void fbgl_mat4Translate(fbgl_Mat4* rhs, float x, float y, float z);
void fbgl_mat4RotateX(fbgl_Mat4* rhs, float angle);
void fbgl_mat4RotateY(fbgl_Mat4* rhs, float angle);
void fbgl_mat4RotateZ(fbgl_Mat4* rhs, float angle);
void fbgl_mat4ShearX(fbgl_Mat4* rhs, float amount);
void fbgl_mat4ShearY(fbgl_Mat4* rhs, float amount);

// For these, pass in a matrix or vector res to modify.
void fbgl_mat4xMat4(fbgl_Mat4* lhs, fbgl_Mat4* rhs, fbgl_Mat4* res);
void fbgl_mat4xVec4(fbgl_Mat4* lhs, fbgl_Vec4* rhs, fbgl_Vec4* res);

#ifdef FBGL_SHORT_NAMES
#define mat4Ident(...)		fbgl_mat4Ident(__VA_ARGS__)
#define mat4Scale(...)		fbgl_mat4Scale(__VA_ARGS__)
#define mat4Translate(...)	fbgl_mat4Translate(__VA_ARGS__)
#define mat4RotateX(...)	fbgl_mat4RotateX(__VA_ARGS__)
#define mat4RotateY(...)	fbgl_mat4RotateY(__VA_ARGS__)
#define mat4RotateZ(...)	fbgl_mat4RotateZ(__VA_ARGS__)
#define mat4ShearX(...)		fbgl_mat4ShearX(__VA_ARGS__)
#define mat4ShearY(...)		fbgl_mat4ShearY(__VA_ARGS__)
#define mat4xMat4(...)		fbgl_mat4xMat4(__VA_ARGS__)
#define mat4xVec4(...)		fbgl_mat4xVec4(__VA_ARGS__)
#endif

#endif
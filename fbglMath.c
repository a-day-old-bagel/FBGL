#include <stdio.h>
#include "fbgl.h"
#include "fbglMath.h"

#define M(mat, row, col, val) mat->m[row][col] = val;
#define FBGL_FILL_MAT(mat, aa, ab, ac, ad, ba, bb, bc, bd,					\
					  ca, cb, cc, cd, da, db, dc, dd)						\
	M(mat,0,0,aa) M(mat,0,1,ab) M(mat,0,2,ac) M(mat,0,3,ad)					\
	M(mat,1,0,ba) M(mat,1,1,bb) M(mat,1,2,bc) M(mat,1,3,bd)					\
	M(mat,2,0,ca) M(mat,2,1,cb) M(mat,2,2,cc) M(mat,2,3,cd)					\
	M(mat,3,0,da) M(mat,3,1,db) M(mat,3,2,dc) M(mat,3,3,dd)
#define P(mat, row, col) mat->m[row][col]
#define FBGL_PRINT_MAT(mat) \
	printf("\n%f,\t%f,\t%f,\t%f,\n" \
			"%f,\t%f,\t%f,\t%f,\n" \
			"%f,\t%f,\t%f,\t%f,\n" \
			"%f,\t%f,\t%f,\t%f,\n", \
			P(mat,0,0),P(mat,0,1),P(mat,0,2),P(mat,0,3), \
			P(mat,1,0),P(mat,1,1),P(mat,1,2),P(mat,1,3), \
			P(mat,2,0),P(mat,2,1),P(mat,2,2),P(mat,2,3), \
			P(mat,3,0),P(mat,3,1),P(mat,3,2),P(mat,3,3));

void fbgl_mat4Ident(fbgl_Mat4* rhs){
	FBGL_FILL_MAT( rhs,
		1.f,   0.f,   0.f,   0.f,
		0.f,   1.f,   0.f,   0.f,
		0.f,   0.f,   1.f,   0.f,
		0.f,   0.f,   0.f,   1.f
	)
}
void fbgl_mat4Scale(fbgl_Mat4* rhs, float x, float y, float z) {
	FBGL_FILL_MAT( rhs,
		x  ,   0.f,   0.f,   0.f,
		0.f,   y  ,   0.f,   0.f,
		0.f,   0.f,   z  ,   0.f,
		0.f,   0.f,   0.f,   1.f
	)
}
void fbgl_mat4Translate(fbgl_Mat4* rhs, float x, float y, float z) {
	FBGL_FILL_MAT( rhs,
		1.f,   0.f,   0.f,   x  ,
		0.f,   1.f,   0.f,   y  ,
		0.f,   0.f,   1.f,   z  ,
		0.f,   0.f,   0.f,   1.f
	)
}
void fbgl_mat4RotateX(fbgl_Mat4* rhs, float angle) {

}
void fbgl_mat4RotateY(fbgl_Mat4* rhs, float angle) {

}
void fbgl_mat4RotateZ(fbgl_Mat4* rhs, float angle) {

}
void fbgl_mat4ShearX(fbgl_Mat4* rhs, float amount) {

}
void fbgl_mat4ShearY(fbgl_Mat4* rhs, float amount) {

}

void fbgl_mat4xMat4(fbgl_Mat4* lhs, fbgl_Mat4* rhs, fbgl_Mat4* res){
	unsigned int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            res->m[i][j] = (lhs->m[i][0] * rhs->m[0][j])
                         + (lhs->m[i][1] * rhs->m[1][j])
                         + (lhs->m[i][2] * rhs->m[2][j])
                         + (lhs->m[i][3] * rhs->m[3][j]);
    //FBGL_PRINT_MAT(res);
}
void fbgl_mat4xVec4(fbgl_Mat4* lhs, fbgl_Vec4* rhs, fbgl_Vec4* res){
	unsigned int i;
	for (i = 0; i < 4; ++i) {
		res->v[i] =  lhs->m[i][0] * rhs->v[0];
		res->v[i] += lhs->m[i][1] * rhs->v[1];
		res->v[i] += lhs->m[i][2] * rhs->v[2];
		res->v[i] += lhs->m[i][3] * rhs->v[3];
	}
}
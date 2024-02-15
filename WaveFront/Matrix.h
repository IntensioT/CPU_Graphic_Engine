#pragma once

#define M_PI       3.14159265358979323846   // pi

class _3DMATRIX
{

public:
	float m[4][4];

	_3DMATRIX();
	_3DMATRIX(float matrix[4][4]);
	_3DMATRIX(float m00, float m10, float m20, float m30,
		float m01, float m11, float m21, float m31,
		float m02, float m12, float m22, float m32,
		float m03, float m13, float m23, float m33);
	~_3DMATRIX();
	_3DMATRIX operator*(_3DMATRIX const& m2);

private:

};




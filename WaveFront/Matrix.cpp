#include "Matrix.h"

_3DMATRIX::_3DMATRIX()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = 0;
		}
	}
}

_3DMATRIX::_3DMATRIX(float matrix[4][4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = matrix[i][j];
		}
	}
}

_3DMATRIX::_3DMATRIX(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33)
{
	m[0][0] = m00;
	m[1][0] = m10;
	m[2][0] = m20;
	m[3][0] = m30;
	m[0][1] = m01;
	m[1][1] = m11;
	m[2][1] = m21;
	m[3][1] = m31;
	m[0][2] = m02;
	m[1][2] = m12;
	m[2][2] = m22;
	m[3][2] = m32;
	m[0][3] = m03;
	m[1][3] = m13;
	m[2][3] = m23;
	m[3][3] = m33;
}

_3DMATRIX::~_3DMATRIX()
{
	delete(this);
}

_3DMATRIX _3DMATRIX::operator*(_3DMATRIX const& m2)
{
	_3DMATRIX result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				result.m[i][j] += this->m[i][k] * m2.m[k][j];
		}
	}
	return result;
}
#include "CoordSystem.h"



CoordSystem::CoordSystem(CoordinateStruct Translation)
{
	XAxis = { 1.0f, 0.0f, 0.0f };
	YAxis = { 0.0f, 1.0f, 0.0f };
	ZAxis = { 0.0f, 0.0f, 1.0f };

	GlobalTransformationMatrix = { XAxis.x, YAxis.x, ZAxis.x, Translation.x,
									XAxis.y, YAxis.y, ZAxis.y, Translation.y,
									XAxis.z, YAxis.z, ZAxis.z, Translation.z,
										0,		0,		 0,		 1 };

}

CoordinateStruct CoordSystem::SubstractVectors(CoordinateStruct& vector1, CoordinateStruct& vector2)
{
	CoordinateStruct result;
	result.x = vector1.x - vector2.x;
	result.y = vector1.y - vector2.y;
	result.z = vector1.z - vector2.z;
	return result;
}

CoordinateStruct CoordSystem::MultiplyVectors(CoordinateStruct& vector1, CoordinateStruct& vector2)
{
	CoordinateStruct result;
	result.x = vector1.x * vector2.x;
	result.y = vector1.y * vector2.y;
	result.z = vector1.z * vector2.z;
	return result;
}

HomogeneousCoordinateStruct CoordSystem::MultiplyMatByVector(_3DMATRIX matrix, HomogeneousCoordinateStruct vector)
{
	float homogeneousVector[4] = { vector.x, vector.y, vector.z, 1.0f };
	float result[4];

	for (int i = 0; i < 4; i++) {
		result[i] = 0.0f;
		for (int j = 0; j < 4; j++) {
			result[i] += matrix.m[i][j] * homogeneousVector[j];
		}
	}

	return { result[0],result[1],result[2], result[3] };
}

_3DMATRIX CoordSystem::MultiplyMatrixByMatrix(const _3DMATRIX& m1, const _3DMATRIX& m2)
{
	_3DMATRIX result;


	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] +
				m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] +
				m1.m[i][3] * m2.m[3][j];
		}
	}


	return result;
}

float CoordSystem::VectorLength(const CoordinateStruct& vector)
{
	return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

CoordinateStruct CoordSystem::NormalizeVector(const CoordinateStruct& vector)
{
	float vectorLength = VectorLength(vector);
	if (vectorLength > 0.0f) {
		float invLength = 1.0f / vectorLength; //  ��������� �������� ���������� ������� �� ���� � �� �� �������� � ������ ���������� �������.
		return { vector.x * invLength, vector.y * invLength, vector.z * invLength };
	}
	else {
		return vector;
	}
}

float CoordSystem::DotProduct(const CoordinateStruct& vector1, const CoordinateStruct& vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

CoordinateStruct CoordSystem::CrossProduct(const CoordinateStruct& vector1, const CoordinateStruct& vector2)
{
	CoordinateStruct result;
	result.x = vector1.y * vector2.z - vector1.z * vector2.y;
	result.y = vector1.z * vector2.x - vector1.x * vector2.z;
	result.z = vector1.x * vector2.y - vector1.y * vector2.x;
	return result;
}

HomogeneousCoordinateStruct CoordSystem::TransformVector(HomogeneousCoordinateStruct& originalVector, CoordinateStruct& scale, float angle, CoordinateStruct& axis, CoordinateStruct& translation)
{
	SetScaleMatrix(scale);
	SetRotationMatrixOptimized(angle, axis);
	SetMovementMatrix(translation);
	_3DMATRIX ModelMatrix = MultiplyMatrixByMatrix(MovementMatrix, MultiplyMatrixByMatrix(RotationMatrix, ScaleMatrix));
	HomogeneousCoordinateStruct res = MultiplyMatByVector(ModelMatrix, originalVector);
	return res;
}

void CoordSystem::SetCameraTransformationMatrix(CoordinateStruct& cameraGlobalCoord, CoordinateStruct& targetGlobalCoord, CoordinateStruct& cameraUpVect)
{
	//CameraTransformationMatrix = GetViewMatrix(cameraGlobalCoord, targetGlobalCoord, cameraUpVect);


	//ZAxis = NormalizeVector(SubstractVectors(cameraGlobalCoord, targetGlobalCoord));
	ZAxis = NormalizeVector(SubstractVectors(targetGlobalCoord, cameraGlobalCoord));
	XAxis = NormalizeVector(CrossProduct(cameraUpVect, ZAxis));
	YAxis = CrossProduct(ZAxis, XAxis);

	float dot1 = -(DotProduct(XAxis, cameraGlobalCoord));
	float dot2 = -(DotProduct(YAxis, cameraGlobalCoord));
	float dot3 = -(DotProduct(ZAxis, cameraGlobalCoord));

	/*CameraTransformationMatrix = { XAxis.x, XAxis.y, XAxis.z, -(DotProduct(XAxis,cameraGlobalCoord)),
									YAxis.x, YAxis.y, YAxis.z, -(DotProduct(YAxis,cameraGlobalCoord)),
									ZAxis.x, ZAxis.y, ZAxis.z, -(DotProduct(ZAxis,cameraGlobalCoord)),
										0,		0,		 0,		 1 };*/


		CameraTransformationMatrix = { XAxis.x, YAxis.x, ZAxis.x, 0,
										XAxis.y, YAxis.y, ZAxis.y, 0,
										XAxis.z, YAxis.z, ZAxis.z, 0,
										dot1,	 dot2,		dot3, 1 };

}

_3DMATRIX CoordSystem::GetViewMatrix(CoordinateStruct& Pos, CoordinateStruct& Target, CoordinateStruct& UpVect)
{
	Position = Pos;
	Look = Target;
	Up = UpVect;

	//Right = Up.Cross(Look);
	Right = CrossProduct(Up, Look);
	//Right.Normalize();
	Right = NormalizeVector(Right);



	// Keep camera's axes orthogonal to eachother
	//Look.Normalize();
	Look = NormalizeVector(Look);

	//Up = Look.Cross(Right);
	Up = CrossProduct(Look, Right);
	//Up.Normalize();
	Up = NormalizeVector(Up);

	//Right = Up.Cross(Look);
	Right = CrossProduct(Up, Look);
	//Right.Normalize();
	Right = NormalizeVector(Right);

	CoordinateStruct R;
	R.x = Right.x; R.y = Right.y; R.z = Right.z;
	CoordinateStruct U;
	U.x = Up.x; U.y = Up.y; U.z = Up.z;
	CoordinateStruct L;
	L.x = Pos.x - Look.x; L.y = Pos.y - Look.y; L.z = Pos.z - Look.z;
	CoordinateStruct P;
	P.x = Position.x; P.y = Position.y; P.z = Position.z;

	L = NormalizeVector(L);

	U = CrossProduct(L, R);
	U = NormalizeVector(U);

	R = CrossProduct(U, L);
	R = NormalizeVector(R);

	float X = -DotProduct(R, P);
	float Y = -DotProduct(U, P);
	float Z = -DotProduct(L, P);

	_3DMATRIX T;

	T.m[0][0] = R.x; T.m[0][1] = U.x; T.m[0][2] = L.x; T.m[0][3] = 0;
	T.m[1][0] = R.y; T.m[1][1] = U.y; T.m[1][2] = L.y; T.m[1][3] = 0;
	T.m[2][0] = R.z; T.m[2][1] = U.z; T.m[2][2] = L.z; T.m[2][3] = 0;
	T.m[3][0] = X; T.m[3][1] = Y;	  T.m[3][2] = Z;	   T.m[3][3] = 1;

	return T;
}

void CoordSystem::SetProjectionTransformationMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	_3DMATRIX result;

	float radianFov = (fov / 2.0f) * (M_PI / 180.0f);

	float tanHalfFov = std::tan(fov / 2.0f);
	//float tanHalfFov =  tan(fov * 0.5f);
	float rangeInv = 1.0f / (nearPlane - farPlane);



	/*result = { 1.0f / (aspectRatio * tanHalfFov),	0,					0,						0,
				0,									1.0f / tanHalfFov,	0,						0,
				0,									0,					rangeInv* farPlane, (nearPlane * farPlane)* rangeInv,
				0,									0,					-1,						0 };*/

	/*result = { (1.0f / (aspectRatio * tanHalfFov)),	0,					0,													0,
				0,									1.0f / tanHalfFov,	0,													0,
				0,									0,					-(farPlane + nearPlane) / (farPlane - nearPlane),	-1,
				0,									0,					-(2 * farPlane * nearPlane) / (farPlane - nearPlane),0 };*/

	result = { (1.0f / (aspectRatio * tanHalfFov)),	0,					0,													0,
				0,									1.0f / tanHalfFov,	0,													0,
				0,									0,					(farPlane) / (farPlane - nearPlane),				-1,
				0,									0,					(farPlane * nearPlane) / (farPlane - nearPlane),	0 };

	ProjectionTransformationMatrix = result;
}

void CoordSystem::SetViewPortTransformationMatrix(float width, float height, float x, float y, float zMin, float zMax)
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	/*ViewPortTransformationMatrix = { halfWidth, 0, 0, x + halfWidth,
										0, halfHeight, 0, halfHeight + y,
										0, 0, (zMax - zMin), zMin,
										0, 0, 0,	1 };*/
	
	ViewPortTransformationMatrix = { halfWidth,				0,					0,			0,
										0,				halfHeight,				0,			0,
										0,					0,				(zMax - zMin),	0,
										x + halfWidth, halfHeight + y,		 zMin,			1 };
}



//void CoordSystem::MoveSystem(CoordinateStruct vect)
//{
//	Center = AddVectors(Center, vect);
//}

void CoordSystem::SetMovementMatrix(CoordinateStruct Translation)
{
	MovementMatrix = { 1, 0, 0, Translation.x,
						0, 1, 0, Translation.y,
						0, 0, 1, Translation.z,
						0, 0, 0,	1 };
}

void CoordSystem::SetScaleMatrix(CoordinateStruct Scale)
{
	ScaleMatrix = { Scale.x,	0,				0,		0,
						0,			Scale.y,		0,		0,
						0,			0,			Scale.z,	0,
						0,			0,				0,		1 };
}

void CoordSystem::SetRotateXMatrix(float angle)
{
	RotateXMatrix = { 1,		0,			0,		0,
						0, cos(angle), -sin(angle), 0,
						0, sin(angle),	cos(angle),	0,
						0,		0,			0,		1 };
}

void CoordSystem::SetRotateYMatrix(float angle)
{
	RotateYMatrix = { cos(angle),		0,			sin(angle),		0,
						0,				1,				0,			0,
						-sin(angle),	0,			cos(angle),		0,
						0,				0,				0,			1 };
}

void CoordSystem::SetRotateZMatrix(float radianAngle)
{

	RotateZMatrix = { cos(radianAngle),		-sin(radianAngle),0,0,
						sin(radianAngle),		 cos(radianAngle),0,0,
						0,					0,		1,0,
						0,					0,		0,1 };
}

void CoordSystem::SetRotateMatrix(float angle, CoordinateStruct axis)
{
	SetRotateXMatrix(angle);
	SetRotateYMatrix(angle);
	SetRotateZMatrix(angle);

	RotationMatrix = MultiplyMatrixByMatrix(MultiplyMatrixByMatrix(RotateXMatrix, RotateYMatrix), RotateZMatrix);
	// ����������� ��� ��������
	float axisLength = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	CoordinateStruct normalizedAxis = { axis.x / axisLength, axis.y / axisLength, axis.z / axisLength };


}

void CoordSystem::SetRotationMatrixOptimized(float angle, const CoordinateStruct& axis)
{
	float rcos = std::cos(angle);
	float rsin = std::sin(angle);

	float u = axis.x;
	float v = axis.y;
	float w = axis.z;

	RotationMatrix = { rcos + u * u * (1 - rcos), w * rsin + v * u * (1 - rcos), -v * rsin + w * u * (1 - rcos), 0.0f,
					   -w * rsin + u * v * (1 - rcos), rcos + v * v * (1 - rcos), u * rsin + w * v * (1 - rcos), 0.0f,
					   v * rsin + u * w * (1 - rcos), -u * rsin + v * w * (1 - rcos), rcos + w * w * (1 - rcos), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f };

	/*rcos = cos(phi);
	rsin = sin(phi);
	matrix[0][0] = rcos + u * u * (1 - rcos);
	matrix[1][0] = w * rsin + v * u * (1 - rcos);
	matrix[2][0] = -v * rsin + w * u * (1 - rcos);
	matrix[0][1] = -w * rsin + u * v * (1 - rcos);
	matrix[1][1] = rcos + v * v * (1 - rcos);
	matrix[2][1] = u * rsin + w * v * (1 - rcos);
	matrix[0][2] = v * rsin + u * w * (1 - rcos);
	matrix[1][2] = -u * rsin + v * w * (1 - rcos);
	matrix[2][2] = rcos + w * w * (1 - rcos);*/
}

_3DMATRIX _3DMATRIX::operator*(const _3DMATRIX& a)
{
	_3DMATRIX result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0.f;
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m[i][k] * a.m[k][j]; // rows * col
			}
		}
	}
	return result;
}

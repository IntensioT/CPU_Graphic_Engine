#include "CoordSystem.h"


HomogeneousCoordinateStruct CoordSystem::ToGlobalCoords(HomogeneousCoordinateStruct local)
{
	//return AddVectors((this->MultiplyMatByVector(GlobalTransformationMatrix,local)), Center);
	HomogeneousCoordinateStruct cent = { Center.x, Center.y, Center.z, 1.0f };
	return AddHomogeneousVectors((this->MultiplyMatByVector(*GlobalTransformationMatrix, local)), cent);
}

CoordSystem::CoordSystem(CoordinateStruct Translation)
{
	XAxis = { 1.0f, 0.0f, 0.0f };
	YAxis = { 0.0f, 1.0f, 0.0f };
	ZAxis = { 0.0f, 0.0f, 1.0f };

	/*GlobalTransformationMatrix = { XAxis.x, YAxis.x, ZAxis.x, Translation.x,
									XAxis.y, YAxis.y, ZAxis.y, Translation.y,
									XAxis.z, YAxis.z, ZAxis.z, Translation.z,
										0,		0,		 0,		 1 };*/

	GlobalTransformationMatrix = new _3DMATRIX( XAxis.x, YAxis.x, ZAxis.x, 0.0f,
									XAxis.y, YAxis.y, ZAxis.y, 0.0f,
									XAxis.z, YAxis.z, ZAxis.z, 0.0f,
									Translation.x,	Translation.y, Translation.z, 1.0f );

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

_3DMATRIX CoordSystem::ColumnMatrixMultiply(const _3DMATRIX& m1, const _3DMATRIX& m2)
{
	_3DMATRIX result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				result.m[i][j] += m1.m[k][j] * m2.m[i][k];
		}
	}
	return result;
}

_3DMATRIX CoordSystem::RowMatrixMultiply(const _3DMATRIX& m1, const _3DMATRIX& m2)
{
	_3DMATRIX result = {0};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
		}
	}
	return result;
}

HomogeneousCoordinateStruct CoordSystem::ColumnMultMatVect(const _3DMATRIX& matrix, HomogeneousCoordinateStruct vector)
{
	HomogeneousCoordinateStruct t;
	/*for(int i=0;i<4;i++){
		for(int j=0;j<4;j++)
			t[i] += p[j] * m.mat[j][i];
	}*/
	t.x = matrix.m[0][0] * vector.x + matrix.m[1][0] * vector.y + matrix.m[2][0] * vector.z + matrix.m[3][0] * vector.w;
	t.y = matrix.m[0][1] * vector.x + matrix.m[1][1] * vector.y + matrix.m[2][1] * vector.z + matrix.m[3][1] * vector.w;
	t.z = matrix.m[0][2] * vector.x + matrix.m[1][2] * vector.y + matrix.m[2][2] * vector.z + matrix.m[3][2] * vector.w;
	t.w = matrix.m[0][3] * vector.x + matrix.m[1][3] * vector.y + matrix.m[2][3] * vector.z + matrix.m[3][3] * vector.w;
	return t;
}

float CoordSystem::VectorLength(const CoordinateStruct& vector)
{
	return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

CoordinateStruct CoordSystem::NormalizeVector(const CoordinateStruct& vector)
{
	float vectorLength = VectorLength(vector);
	if (vectorLength > 0.0f) {
		float invLength = 1.0f / vectorLength; //  позволяет избежать повторного деления на одно и то же значение в каждой компоненте вектора.
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
	//_3DMATRIX ModelMatrix = MultiplyMatrixByMatrix(MovementMatrix, MultiplyMatrixByMatrix(RotationMatrix, ScaleMatrix));
	HomogeneousCoordinateStruct res; /*= MultiplyMatByVector(ModelMatrix, originalVector);*/
	return res;
}

_3DMATRIX CoordSystem::TransformMatrix(_3DMATRIX originalMatrix, CoordinateStruct& scale, float angle, CoordinateStruct& axis, CoordinateStruct& translation)
{
	SetScaleMatrix(scale);
	SetRotationMatrixOptimized(angle, axis);
	SetMovementMatrix(translation);

	/*_3DMATRIX ModelMatrix = MovementMatrix * RotationMatrix * ScaleMatrix;
	_3DMATRIX res = ModelMatrix * originalMatrix;*/
	
	/*_3DMATRIX ModelMatrix = ScaleMatrix * RotationMatrix * MovementMatrix;
	HomogeneousCoordinateStruct temp;

	for (int i = 0; i < 4; i++)
	{
			temp *= ScaleMatrix;
			temp *= RotationMatrix;
			temp *= MovementMatrix;
	}*/
	_3DMATRIX res/* = originalMatrix * ModelMatrix*/;
	return res;
}

void CoordSystem::SetCameraTransformationMatrix(CoordinateStruct& cameraGlobalCoord, CoordinateStruct& targetGlobalCoord, CoordinateStruct& cameraUpVect)
{
	//row
	//CameraTransformationMatrix = GetViewMatrix(cameraGlobalCoord, targetGlobalCoord, cameraUpVect);

	//column
	ZAxis = (SubstractVectors(targetGlobalCoord, cameraGlobalCoord));
	ZAxis = NormalizeVector(ZAxis);
	XAxis = CrossProduct(ZAxis, cameraUpVect);
	XAxis = NormalizeVector(XAxis);
	YAxis = CrossProduct(XAxis, ZAxis);


	float sus1 = -(DotProduct(XAxis, cameraGlobalCoord));
	float sus2 = -(DotProduct(YAxis, cameraGlobalCoord));
	float sus3 = -(DotProduct(ZAxis, cameraGlobalCoord));

	//Column
	/*CameraTransformationMatrix = { XAxis.x, XAxis.y, XAxis.z, sus1,
									YAxis.x, YAxis.y, YAxis.z, sus2,
									ZAxis.x, ZAxis.y, ZAxis.z, sus3,
										0,		0,		 0,		 1 };*/
	//row
	CameraTransformationMatrix = new _3DMATRIX( XAxis.x,		YAxis.x, ZAxis.x,	0,
									XAxis.y,	YAxis.y, ZAxis.y,	0,
									XAxis.z,	YAxis.z, ZAxis.z,	0,
									sus1,		sus2,		sus3,		 1 );

	/*vec<3, T, Q> const f(normalize(center - eye));
	vec<3, T, Q> const s(normalize(cross(f, up)));
	vec<3, T, Q> const u(cross(s, f));

	mat<4, 4, T, Q> Result(1);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -dot(s, eye);
	Result[3][1] = -dot(u, eye);
	Result[3][2] = dot(f, eye);
	return Result;*/

}

_3DMATRIX CoordSystem::GetViewMatrix(CoordinateStruct& Pos, CoordinateStruct& Target, CoordinateStruct& UpVect)
{
	Position = Pos;
	Look = Target;
	Up = UpVect;

	Right = CrossProduct(Up, Look);
	Right = NormalizeVector(Right);

	// Keep camera's axes orthogonal to eachother
	Look = NormalizeVector(Look);

	Up = CrossProduct(Look, Right);
	Up = NormalizeVector(Up);

	Right = CrossProduct(Up, Look);
	Right = NormalizeVector(Right);

	CoordinateStruct R;
	R.x = Right.x; R.y = Right.y; R.z = Right.z;
	CoordinateStruct U;
	U.x = Up.x; U.y = Up.y; U.z = Up.z;
	CoordinateStruct L;
	L.x = Look.x; L.y = Look.y; L.z = Look.z;
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
	T.m[3][0] = X; T.m[3][1] = Y; T.m[3][2] = Z; T.m[3][3] = 1;

	return T;
}

void CoordSystem::SetProjectionTransformationMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	_3DMATRIX* result;

	float radianFov = (fov / 2.0f) * (M_PI / 180.0f);

	float tanHalfFov = std::tan(fov / 2.0f);
	//float tanHalfFov =  tan(fov * 0.5f);
	float rangeInv = 1.0f / (nearPlane - farPlane);


	//row
	result = new _3DMATRIX ( (1.0f / (aspectRatio * tanHalfFov)),	0,					0,													0,
				0,									1.0f / tanHalfFov,	0,													0,
				0,									0,					(farPlane) / (farPlane - nearPlane),				1,
				0,									0,					-(farPlane * nearPlane) / (farPlane - nearPlane),	0 );

	////column
	//result = { (1.0f / (aspectRatio * tanHalfFov)),	0,					0,													0,
	//			0,									1.0f / tanHalfFov,	0,													0,
	//			0,									0,					(farPlane) / (farPlane - nearPlane),		-(farPlane * nearPlane) / (farPlane - nearPlane),
	//			0,									0,					1,	0 };

	ProjectionTransformationMatrix = result;
}

void CoordSystem::SetViewPortTransformationMatrix(float width, float height, float x, float y, float zMin, float zMax)
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;


	//row
	ViewPortTransformationMatrix = new _3DMATRIX( halfWidth,				0,					0,			0,
										0,				halfHeight,				0,			0,
										0,					0,				(zMax - zMin),	0,
										x + halfWidth, halfHeight + y,		 zMin,			1 );

	////column
	//ViewPortTransformationMatrix = { halfWidth,				0,					0,			x + halfWidth,
	//									0,				halfHeight,				0,			halfHeight + y,
	//									0,					0,				(zMax - zMin),	zMin,
	//									0,					0,					0,			1 };
}



void CoordSystem::MoveSystem(CoordinateStruct vect)
{
	Center = AddVectors(Center, vect);
}

void CoordSystem::SetMovementMatrix(CoordinateStruct Translation)
{
	MovementMatrix = new _3DMATRIX( 1, 0, 0, Translation.x,
						0, 1, 0, Translation.y,
						0, 0, 1, Translation.z,
						0, 0, 0,	1 );
}

void CoordSystem::SetScaleMatrix(CoordinateStruct Scale)
{
	ScaleMatrix = new _3DMATRIX( Scale.x,	0,				0,		0,
						0,			Scale.y,		0,		0,
						0,			0,			Scale.z,	0,
						0,			0,				0,		1 );
}

void CoordSystem::SetRotateXMatrix(float angle)
{
	RotateXMatrix = new _3DMATRIX ( 1,		0,			0,		0,
						0, cos(angle), -sin(angle), 0,
						0, sin(angle),	cos(angle),	0,
						0,		0,			0,		1 );
}

void CoordSystem::SetRotateYMatrix(float angle)
{
	RotateYMatrix =  new _3DMATRIX( cos(angle),		0,			sin(angle),		0,
						0,				1,				0,			0,
						-sin(angle),	0,			cos(angle),		0,
						0,				0,				0,			1 );
}

void CoordSystem::SetRotateZMatrix(float radianAngle)
{

	RotateZMatrix = new _3DMATRIX( cos(radianAngle),		-sin(radianAngle),0,0,
						sin(radianAngle),		 cos(radianAngle),0,0,
						0,					0,		1,0,
						0,					0,		0,1 );
}

void CoordSystem::SetRotateMatrix(float angle, CoordinateStruct axis)
{
	SetRotateXMatrix(angle);
	SetRotateYMatrix(angle);
	SetRotateZMatrix(angle);

	//RotationMatrix = MultiplyMatrixByMatrix(MultiplyMatrixByMatrix(RotateXMatrix, RotateYMatrix), RotateZMatrix);
	// Нормализуем ось вращения
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

	RotationMatrix = new _3DMATRIX ( rcos + u * u * (1 - rcos), w * rsin + v * u * (1 - rcos), -v * rsin + w * u * (1 - rcos), 0.0f,
					   -w * rsin + u * v * (1 - rcos), rcos + v * v * (1 - rcos), u * rsin + w * v * (1 - rcos), 0.0f,
					   v * rsin + u * w * (1 - rcos), -u * rsin + v * w * (1 - rcos), rcos + w * w * (1 - rcos), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f );

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


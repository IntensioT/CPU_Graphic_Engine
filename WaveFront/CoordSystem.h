#pragma once
#include "ObjLoader.h"
#include "Matrix.h"




//typedef struct _3DMATRIX {
//    union {
//        struct {
//            float        _11, _12, _13, _14;
//            float        _21, _22, _23, _24;
//            float        _31, _32, _33, _34;
//            float        _41, _42, _43, _44;
//
//        };
//        float m[4][4];
//    };
//}; 

struct HomogeneousCoordinateStruct
{
    float x, y, z, w;

    HomogeneousCoordinateStruct& operator*=(const float f) {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }
};


inline CoordinateStruct AddVectors(const CoordinateStruct& vector1, const CoordinateStruct& vector2) {
    CoordinateStruct result;
    result.x = vector1.x + vector2.x;
    result.y = vector1.y + vector2.y;
    result.z = vector1.z + vector2.z;
    return result;
}

inline HomogeneousCoordinateStruct AddHomogeneousVectors(const HomogeneousCoordinateStruct& vector1, const HomogeneousCoordinateStruct& vector2) {
    HomogeneousCoordinateStruct result;
    result.x = vector1.x + vector2.x;
    result.y = vector1.y + vector2.y;
    result.z = vector1.z + vector2.z;
    result.w = vector1.w + vector2.w;
    return result;
}

//inline _3DMATRIX operator*(_3DMATRIX const& m1, _3DMATRIX const& m2)
//{
//    _3DMATRIX result = {0};
//
//    for (int i = 0; i < 4; i++) {
//        for (int j = 0; j < 4; j++) {
//            for (int k = 0; k < 4; k++)
//                result.m[i][j] += m1.m[i][k] * m2.m[k][j];
//        }
//    }
//    return result;
//}

inline HomogeneousCoordinateStruct operator* (HomogeneousCoordinateStruct& p, const _3DMATRIX& m) 
{
    HomogeneousCoordinateStruct t = {0};
    /*for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            t[i] += p[j] * m.mat[j][i];
    }*/
    t.x = p.x * m.m[0][0] + p.y * m.m[1][0] + p.z * m.m[2][0] + p.w * m.m[3][0];
    t.y = p.x * m.m[0][1] + p.y * m.m[1][1] + p.z * m.m[2][1] + p.w * m.m[3][1];
    t.z = p.x * m.m[0][2] + p.y * m.m[1][2] + p.z * m.m[2][2] + p.w * m.m[3][2];
    t.w = p.x * m.m[0][3] + p.y * m.m[1][3] + p.z * m.m[2][3] + p.w * m.m[3][3];

    /*t.x = p.x * m.m[0][0] + p.y * m.m[0][1] + p.z * m.m[0][2] + p.w * m.m[0][3];
    t.y = p.x * m.m[1][0] + p.y * m.m[1][1] + p.z * m.m[1][2] + p.w * m.m[1][3];
    t.z = p.x * m.m[2][0] + p.y * m.m[2][1] + p.z * m.m[2][2] + p.w * m.m[2][3];
    t.w = p.x * m.m[3][0] + p.y * m.m[3][1] + p.z * m.m[3][2] + p.w * m.m[3][3];*/
    return t;
}

inline HomogeneousCoordinateStruct operator*=(HomogeneousCoordinateStruct& p, const _3DMATRIX& m) {
    p = p * m;
    return p;
}


inline float GetRadians(float angle)
{
    return (angle) * (M_PI / 180.0f);
}

class CoordSystem
{
    CoordinateStruct Center;
    CoordinateStruct XAxis, YAxis, ZAxis;


public:
    _3DMATRIX* GlobalTransformationMatrix;
    _3DMATRIX* CameraTransformationMatrix;
    _3DMATRIX* ProjectionTransformationMatrix;
    _3DMATRIX* ViewPortTransformationMatrix;

    _3DMATRIX* MovementMatrix;

    _3DMATRIX* ScaleMatrix;

    _3DMATRIX* RotateXMatrix;
    _3DMATRIX* RotateYMatrix;
    _3DMATRIX* RotateZMatrix;

    _3DMATRIX* RotationMatrix;


    HomogeneousCoordinateStruct ToGlobalCoords(HomogeneousCoordinateStruct local);
    CoordinateStruct ToLocalCoords(CoordinateStruct global);
    CoordSystem(CoordinateStruct Translation);

    CoordinateStruct SubstractVectors(CoordinateStruct& vector1, CoordinateStruct& vector2);
    CoordinateStruct MultiplyVectors(CoordinateStruct& vector1, CoordinateStruct& vector2);
    HomogeneousCoordinateStruct MultiplyMatByVector(_3DMATRIX matrix, HomogeneousCoordinateStruct vector);
    _3DMATRIX MultiplyMatrixByMatrix(const _3DMATRIX& mat1, const _3DMATRIX& mat2);
    _3DMATRIX ColumnMatrixMultiply(const _3DMATRIX& m1, const _3DMATRIX& m2);
    _3DMATRIX RowMatrixMultiply(const _3DMATRIX& m1, const _3DMATRIX& m2);
    HomogeneousCoordinateStruct ColumnMultMatVect(const _3DMATRIX& matrix, HomogeneousCoordinateStruct vector);

    float DotProduct(const CoordinateStruct& vector1, const CoordinateStruct& vector2);
    CoordinateStruct CrossProduct(const CoordinateStruct& vector1, const CoordinateStruct& vector2);



    float VectorLength(const CoordinateStruct& vector);
    CoordinateStruct NormalizeVector(const CoordinateStruct& vector);

    HomogeneousCoordinateStruct TransformVector(HomogeneousCoordinateStruct& originalVector, CoordinateStruct& scale, float angle, CoordinateStruct& axis, CoordinateStruct& translation);
    _3DMATRIX TransformMatrix(_3DMATRIX originalMatrix, CoordinateStruct& scale, float angle, CoordinateStruct& axis, CoordinateStruct& translation);



    void SetCameraTransformationMatrix(CoordinateStruct& cameraGlobalCoord, CoordinateStruct& targetGlobalCoord, CoordinateStruct& cameraUpVect);
    _3DMATRIX GetViewMatrix(CoordinateStruct& Pos, CoordinateStruct& Target, CoordinateStruct& UpVect);
    void SetProjectionTransformationMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
    void SetViewPortTransformationMatrix(float width, float height, float x, float y, float zMin, float zMax);

    void MoveSystem(CoordinateStruct vect);

    void SetMovementMatrix(CoordinateStruct Translation);
    void SetScaleMatrix(CoordinateStruct Scale);
    void SetRotateXMatrix(float angle);
    void SetRotateYMatrix(float angle);
    void SetRotateZMatrix(float angle);
    void SetRotateMatrix(float angle, CoordinateStruct Axis);
    void SetRotationMatrixOptimized(float angle, const CoordinateStruct& axis);



private:
	_3DMATRIX TransformationMatrix;
	_3DMATRIX LocalTransformationMatrix;

    CoordinateStruct Right;
    CoordinateStruct Up;
    CoordinateStruct Look;

    CoordinateStruct Position;
};


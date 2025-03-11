#pragma once

#include<DirectXMath.h>
using namespace DirectX;

/**********************************
XMFLOAT2ââéZéq
**********************************/

XMFLOAT2 operator+(const XMFLOAT2& a, const XMFLOAT2& b);
XMFLOAT2 operator-(const XMFLOAT2& a, const XMFLOAT2& b);
XMFLOAT2 operator*(const XMFLOAT2& a, const float b);
XMFLOAT2 operator*(const float a, const XMFLOAT2& b);
void operator+=(XMFLOAT2& a, const XMFLOAT2& b);
void operator-=(XMFLOAT2& a, const XMFLOAT2& b);
void operator*=(XMFLOAT2& a, float b);

/**********************************
XMFLOAT3ââéZéq
**********************************/

XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b);
XMFLOAT3 operator-(const XMFLOAT3& a, const XMFLOAT3& b);
XMFLOAT3 operator*(const XMFLOAT3& a, const float b);
XMFLOAT3 operator*(const float a, const XMFLOAT3& b);
XMFLOAT3 operator*(const XMFLOAT3& vec, const XMMATRIX& mat);
XMFLOAT3 operator*(const XMFLOAT4& vec, const XMMATRIX& mat);
void operator+=(XMFLOAT3& a, const XMFLOAT3& b);
void operator-=(XMFLOAT3& a, const XMFLOAT3& b);
void operator*=(XMFLOAT3& a, float b);

/**********************************
íËêî
**********************************/

const XMFLOAT2 UnitVec2X(1.0f, 0.0f);
const XMFLOAT3 UnitVec3X(1.0f, 0.0f, 0.0f);
const XMFLOAT2 UnitVec2Y(0.0f, 1.0f);
const XMFLOAT3 UnitVec3Y(0.0f, 1.0f, 0.0f);
const XMFLOAT3 UnitVec3Z(0.0f, 0.0f, 1.0f);
const XMFLOAT2 ZeroVec2(0.0f, 0.0f);
const XMFLOAT3 ZeroVec3(0.0f, 0.0f, 0.0f);
const XMFLOAT2 OneVec2(1.0f, 1.0f);
const XMFLOAT3 OneVec3(1.0f, 1.0f, 1.0f);

/**********************************
XMFLOAT2ånä÷êî
**********************************/

XMFLOAT2 UnitVecAngle(float theta);

float Length(const XMFLOAT2& a);

float LengthSq(const XMFLOAT2& a);

XMFLOAT2 Normalize(const XMFLOAT2& a);

/**********************************
XMFLOAT3ånä÷êî
**********************************/

float Length(const XMFLOAT3& a);

float LengthSq(const XMFLOAT3& a);

XMFLOAT3 Normalize(const XMFLOAT3& a);

float Dot(const XMFLOAT2& a, const XMFLOAT2& b);

float Cross(const XMFLOAT2& a, const XMFLOAT2& b);

float Dot(const XMFLOAT3& a, const XMFLOAT3& b);

XMFLOAT3 Cross(const XMFLOAT3& a, const XMFLOAT3& b);

float CalcLengthProjAxis(const XMFLOAT3& axis,
	const XMFLOAT3& bx, const XMFLOAT3& by, const XMFLOAT3& bz);

XMFLOAT3 CalcNormalVector(const XMFLOAT3& vector1, const XMFLOAT3& vector2);

/// <summary>
/// *ââéZéqÇ∆ÇÕàŸÇ»ÇËwèúéZÇ…ëŒâûÇµÇΩÉxÉNÉgÉãÇ∆çsóÒÇÃä|ÇØéZÇçsÇ§
/// </summary>
XMFLOAT3 CalcMatMul(const XMFLOAT3& vec, const XMMATRIX& mat);

/// <summary>
/// *ââéZéqÇ∆ÇÕàŸÇ»ÇËwèúéZÇ…ëŒâûÇµÇΩÉxÉNÉgÉãÇ∆çsóÒÇÃä|ÇØéZÇçsÇ§
/// </summary>
XMFLOAT3 CalcMatMul(const XMFLOAT4& vec, const XMMATRIX& mat);

XMFLOAT3 Lerp(const XMFLOAT3& start, const XMFLOAT3& end, float t);
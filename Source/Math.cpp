#include "Math.h"
#include <cmath>

/**********************************
XMFLOAT2‰‰ŽZŽq
**********************************/

XMFLOAT2 operator+(const XMFLOAT2& a, const XMFLOAT2& b) 
{
	return XMFLOAT2(a.x + b.x, a.y + b.y); 
}

XMFLOAT2 operator-(const XMFLOAT2& a, const XMFLOAT2& b) 
{
	return XMFLOAT2(a.x - b.x, a.y - b.y); 
}

XMFLOAT2 operator*(const XMFLOAT2& a, const float b) 
{
	return XMFLOAT2(a.x * b, a.y * b); 
}

XMFLOAT2 operator*(const float a, const XMFLOAT2& b) 
{	
	return XMFLOAT2(a * b.x, a * b.y);
}

void operator+=(XMFLOAT2& a, const XMFLOAT2& b) 
{
	a.x += b.x;
	a.y += b.y;
}

void operator-=(XMFLOAT2& a, const XMFLOAT2& b)
{
	a.x -= b.x;
	a.y -= b.y;
}

void operator*=(XMFLOAT2& a, float b)
{
	a.x *= b;
	a.y *= b;
}

/**********************************
XMFLOAT3‰‰ŽZŽq
**********************************/

XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

XMFLOAT3 operator-(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
}

XMFLOAT3 operator*(const XMFLOAT3& a, const float b)
{
	return XMFLOAT3(a.x * b, a.y * b, a.z * b);
}

XMFLOAT3 operator*(const float a, const XMFLOAT3& b)
{
	return XMFLOAT3(b.x * a, b.y * a, b.z * a);
}

XMFLOAT3 operator*(const XMFLOAT3& vec, const XMMATRIX& mat)
{
	float v[4] = { vec.x,vec.y,vec.z,1.0f };
	float output[3]{};

	for (int i = 0;i < 3;i++)
	{
		output[i] = 0.0f;
		for (int j = 0;j < 4;j++)
		{
			output[i] += v[j] * mat.r[j].m128_f32[i];
		}
	}

	return XMFLOAT3(output[0], output[1], output[2]);
}

XMFLOAT3 operator*(const XMFLOAT4& vec, const XMMATRIX& mat)
{
	float v[4] = { vec.x,vec.y,vec.z,vec.w };
	float output[3]{};

	for (int i = 0;i < 3;i++)
	{
		output[i] = 0.0f;
		for (int j = 0;j < 4;j++)
		{
			output[i] += v[j] * mat.r[j].m128_f32[i];
		}
	}

	return XMFLOAT3(output[0], output[1], output[2]);
}

void operator+=(XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

void operator-=(XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}

void operator*=(XMFLOAT3& a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
}

/**********************************
XMFLOAT2ŒnŠÖ”
**********************************/

XMFLOAT2 UnitVecAngle(float theta) 
{
	return XMFLOAT2(std::cos(theta), std::sin(theta)); 
}

float Length(const XMFLOAT2& a)
{
	return std::sqrt(a.x * a.x + a.y * a.y);
}

float LengthSq(const XMFLOAT2& a)
{
	return a.x * a.x + a.y * a.y;
}

XMFLOAT2 Normalize(const XMFLOAT2& a)
{
	float len = Length(a);
	if (len < FLT_EPSILON) return XMFLOAT2(0.0f, 0.0f);

	float inLen = 1.0f / len;
	return XMFLOAT2(a.x * inLen, a.y * inLen);
}

float Dot(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return a.x * b.x + a.y * b.y;
}

float Cross(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return a.x * b.y - a.y * b.x;
}

/**********************************
XMFLOAT3ŒnŠÖ”
**********************************/

float Length(const XMFLOAT3& a)
{
	return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

float LengthSq(const XMFLOAT3& a)
{
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

XMFLOAT3 Normalize(const XMFLOAT3& a)
{
	float len = Length(a);
	if (std::fabs(len) < FLT_EPSILON) return ZeroVec3;

	XMFLOAT3 b = a;
	float c = 1.0f / len;
	return a * c;
}

float Dot(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

XMFLOAT3 Cross(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3
	(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

float CalcLengthProjAxis(const XMFLOAT3& axis,
	const XMFLOAT3& bx, const XMFLOAT3& by, const XMFLOAT3& bz)
{
	float s1 = std::abs(Dot(axis, bx));
	float s2 = std::abs(Dot(axis, by));
	float s3 = std::abs(Dot(axis, bz));

	return s1 + s2 + s3;
}

XMFLOAT3 CalcNormalVector(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
{
	return Normalize(Cross(vector1, vector2));
}

XMFLOAT3 CalcMatMul(const XMFLOAT3& vec, const XMMATRIX& mat)
{
	float v[4] = { vec.x,vec.y,vec.z,1.0f };
	float output[4]{};

	for (int i = 0;i < 4;i++)
	{
		output[i] = 0.0f;
		for (int j = 0;j < 4;j++)
		{
			output[i] += v[j] * mat.r[j].m128_f32[i];
		}
	}

	float iw = 1.0f / output[3];
	return XMFLOAT3(output[0] * iw, output[1] * iw, output[2] * iw);
}

XMFLOAT3 CalcMatMul(const XMFLOAT4& vec, const XMMATRIX& mat)
{
	float v[4] = { vec.x,vec.y,vec.z,vec.w };
	float output[4]{};

	for (int i = 0;i < 4;i++)
	{
		output[i] = 0.0f;
		for (int j = 0;j < 4;j++)
		{
			output[i] += v[j] * mat.r[j].m128_f32[i];
		}
	}

	float iw = 1.0f / output[3];
	return XMFLOAT3(output[0] * iw, output[1] * iw, output[2] * iw);
}

XMFLOAT3 Lerp(const XMFLOAT3& start, const XMFLOAT3& end, float t)
{
	return XMFLOAT3
	(
		start.x + (end.x - start.x) * t,
		start.y + (end.y - start.y) * t,
		start.z + (end.z - start.z) * t
	);
}
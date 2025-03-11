#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct CamInParam //プロジェクション行列用
{
	CamInParam() :fovY(90.0f), zNear(1.0f), zFar(10.0f) {}
	CamInParam(float ifovY, float iNear, float iFar) :fovY(ifovY), zNear(iNear), zFar(iFar) {}

	float fovY;
	float zNear;
	float zFar;
};

struct CamExtParam //ビュー行列用
{
	CamExtParam() :eye(0.0f, 0.0f, 0.0f), target(1.0f, 1.0f, 1.0f), up(0.0f, 1.0f, 0.0f) {}
	CamExtParam(XMFLOAT3 ieye, XMFLOAT3 itarget, XMFLOAT3 iup) :eye(ieye), target(itarget), up(iup) {}

	XMFLOAT3 eye;
	XMFLOAT3 target;
	XMFLOAT3 up;
};

class Camera
{
public:
	Camera();
	Camera(CamInParam inP, CamExtParam extP);
	~Camera();

	CamInParam  GetInParam() const			  { return m_inP; }
	void	    SetInParam(CamInParam inP)	  { m_inP = inP; }
	CamExtParam GetExtParam() const			  { return m_extP; }
	void	    SetExtParam(CamExtParam extP) { m_extP = extP; }

	XMMATRIX CalcViewMatrix() const;
	XMMATRIX CalcProjMatrix(float w, float h) const;
	XMMATRIX CalcViewProjMatrix(float w, float h) const;

private:
	CamInParam m_inP;
	CamExtParam m_extP;
};

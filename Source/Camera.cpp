#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(CamInParam inP, CamExtParam extP)
	:m_inP(inP)
	, m_extP(extP)
{
}

Camera::~Camera()
{
}

XMMATRIX Camera::CalcViewMatrix() const
{
	return XMMatrixLookAtLH(XMLoadFloat3(&m_extP.eye), XMLoadFloat3(&m_extP.target), XMLoadFloat3(&m_extP.up));
}

XMMATRIX Camera::CalcProjMatrix(float w, float h) const
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_inP.fovY), w / h, m_inP.zNear, m_inP.zFar);
}

XMMATRIX Camera::CalcViewProjMatrix(float w, float h) const
{
	XMMATRIX view = CalcViewMatrix();
	XMMATRIX proj = CalcProjMatrix(w, h);
	return view * proj;
}
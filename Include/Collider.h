#pragma once

#include "Math.h"
#include "GameUtil.h"
using namespace Shape;

/**********************************
�~�Ƃ̏Փ˔���
**********************************/

bool Collide(const Circle& c, const XMFLOAT2& p);

bool Collide(const Circle& c1, const Circle& c2);

bool Collide(const Circle& c, const Segment& s);

/**********************************
��`�Ƃ̏Փ˔���
**********************************/

bool Collide(const Shape::Rect& rect, const XMFLOAT2& p);

bool Collide(const Shape::Rect& rect1, const Shape::Rect& rect2);

bool Collide(const Shape::Rect& rect, const Circle& c);

bool Collide(const Shape::RotateRect& rect, const Circle& c);

/**********************************
���Ƃ̏Փ˔���
**********************************/

bool Collide(const Sphere& s, const XMFLOAT3& p);

bool Collide(const Sphere& s1, const Sphere& s2);

/// <summary>
/// s1���Œ�ƍl���A�߂荞�ݕ␳���s��
/// </summary>
/// <param name="corrDir">s1���߂荞�ݕ␳��������x�N�g��</param>
/// <param name="len">�ړ���</param>
bool Collide(const Sphere& s1, const Sphere& s2,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

bool Collide(const Sphere& sphere, const OBB& obb,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
���ʂƂ̏Փ˔���
**********************************/

/// <summary>
/// plane���Œ�ƍl���Asphere�ɂ߂荞�ݕ␳���s��
/// </summary>
/// <param name="corrDir">s1���߂荞�ݕ␳��������x�N�g��</param>
/// <param name="len">�ړ���</param>
bool Collide(const Sphere& sphere, const InfinitePlane& plane,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
AABB�Ƃ̏Փ˔���
**********************************/

/// <summary>
/// b2���Œ�ƍl���A�߂荞�ݕ␳���s��<para></para>
/// �߂荞�ݕ␳�ʂ̌v�Z���s�v�ȏꍇ��corrDir,len��nullptr��ݒ�
/// </summary>
/// <param name="corrDir">b2���߂荞�ݕ␳��������x�N�g��</param>
/// <param name="len">�ړ���</param>
bool Collide(const AABB& b1, const AABB& b2,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
OBB�Ƃ̏Փ˔���
**********************************/

/// <summary>
/// �߂荞�ݕ␳�ʂ̌v�Z���s�v�ȏꍇ��corrDir,len��nullptr��ݒ�
/// </summary>
/// <param name="corrDir">b2���߂荞�ݕ␳��������x�N�g��</param>
/// <param name="len">�ړ���</param>
bool Collide(const OBB& b1, const OBB& b2,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
Ray�Ƃ̏Փ˔���
**********************************/

/// <param name="vert0,1,2">�O�p�`�̒��_</param>
/// <param name="rho">�Փˎ��̔}��ϐ���ۑ�����ϐ��̃|�C���^</param>
/// <param name="cull">�J�����O�L���t���O �L�����A�O�p�`���ʂւ̃��C���v�Z���Ȃ�</param>
/// <returns></returns>
bool Collide(const Ray& ray, const XMFLOAT3& vert0,
	const XMFLOAT3& vert1, const XMFLOAT3& vert2, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const OBB& obb, float* rho = nullptr);

bool Collide(const Ray& ray, const InfinitePlane& plane, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const FinitePlane& plane, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const Sphere& sphere, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const GridHeightMap& map, float* rho = nullptr);


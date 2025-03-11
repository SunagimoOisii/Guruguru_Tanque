#pragma once

#include "MeshActor.h"

class FileAnimMeshActor : public MeshActor
{
public:
	FileAnimMeshActor(class Game* game, int shaderClassIndex, const wchar_t* filePath,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		const wchar_t* imgFilePrefix = L"image\\", bool ddsFlag = false);
	virtual ~FileAnimMeshActor();

	bool IsEnabled() const;

	virtual void UpdateActor(float deltaTime) override;
	virtual void Draw() override;

private:
	float m_elapsedAnimTime;
};
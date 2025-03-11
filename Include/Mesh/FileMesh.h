#pragma once

#include "Mesh.h"

/// <summary>
/// FbFix�t�@�C��������
/// </summary>
class FileMesh : public Mesh
{
public:
	FileMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* filePath,
		const wchar_t* imgFilePrefix = L"image\\texture\\", bool ddsFlag = false);
	~FileMesh();

	void Draw() override;

private:
	bool ReadFbfixFile(const wchar_t* filePath, const wchar_t* imgFilePrefix,
		std::vector<MeshData>& mData);

	std::vector<int> m_indicesNum;
	XMMATRIX m_adjustMat;
};

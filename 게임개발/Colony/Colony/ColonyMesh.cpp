#include "stdafx.h"
#include "ColonyMesh.h"
#include "ColonyGameObject.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//					Basic , standard , Skinned Mesh Class -> This is Upload fbxObject			   //
//																								   //
//								BasicMesh class -> (Basic Class)								   //
//																								   //
//								Standard Mesh Class -> Static Object 						       //
//																								   //
//								Skinned Mesh Class -> Skinned Ojbect 						       //
/////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										BasicMesh Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
BasicMesh::BasicMesh(ID3D12Device* pd3dDeivce, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

BasicMesh::~BasicMesh()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();

	if (m_nSubMeshes > 0)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexBuffers[i]) m_ppd3dSubSetIndexBuffers[i]->Release();
			if (m_ppnSubSetIndices[i]) delete[] m_ppnSubSetIndices[i];
		}
		if (m_ppd3dSubSetIndexBuffers) delete[] m_ppd3dSubSetIndexBuffers;
		if (m_pd3dSubSetIndexBufferViews) delete[] m_pd3dSubSetIndexBufferViews;

		if (m_pnSubSetIndices) delete[] m_pnSubSetIndices;
		if (m_ppnSubSetIndices) delete[] m_ppnSubSetIndices;
	}

	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
}

void BasicMesh::ReleaseUploadBuffers()
{
	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer->Release();
	m_pd3dPositionUploadBuffer = NULL;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexUploadBuffers) delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void BasicMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);
}

void BasicMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void BasicMesh::OnPostRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										StandardMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
StandardMesh::StandardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : BasicMesh(pd3dDevice, pd3dCommandList)
{
}

StandardMesh::~StandardMesh()
{
	if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	m_pd3dTextureCoord0Buffer = NULL;
	if (m_pd3dTextureCoord1Buffer) m_pd3dTextureCoord1Buffer->Release();
	m_pd3dTextureCoord0Buffer = NULL;
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	m_pd3dNormalBuffer = NULL;
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();
	m_pd3dTangentBuffer = NULL;
	if (m_pd3dBiTangentBuffer) m_pd3dBiTangentBuffer->Release();
	m_pd3dBiTangentBuffer = NULL;

	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf3Tangents) delete[] m_pxmf3Tangents;
	if (m_pxmf3BiTangents) delete[] m_pxmf3BiTangents;
	if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
	if (m_pxmf2TextureCoords1) delete[] m_pxmf2TextureCoords1;
}

void StandardMesh::ReleaseUploadBuffers()
{
	BasicMesh::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dTextureCoord1UploadBuffer) m_pd3dTextureCoord1UploadBuffer->Release();
	m_pd3dTextureCoord1UploadBuffer = NULL;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;

	if (m_pd3dTangentUploadBuffer) m_pd3dTangentUploadBuffer->Release();
	m_pd3dTangentUploadBuffer = NULL;

	if (m_pd3dBiTangentUploadBuffer) m_pd3dBiTangentUploadBuffer->Release();
	m_pd3dBiTangentUploadBuffer = NULL;
}

void StandardMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
			m_BoundingBox.Center = m_xmf3AABBCenter;
			m_BoundingBox.Extents = m_xmf3AABBExtents;
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);

				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				m_d3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				m_pnSubSetIndices = new int[m_nSubMeshes];
				m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

				m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile); //i
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], pInFile);

							m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);

							m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

void StandardMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									SkinnedMesh Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
SkinnedMesh::SkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : StandardMesh(pd3dDevice, pd3dCommandList)
{
}

SkinnedMesh::~SkinnedMesh()
{
	if (m_pxmn4BoneIndices) delete[] m_pxmn4BoneIndices;
	if (m_pxmf4BoneWeights) delete[] m_pxmf4BoneWeights;

	if (m_ppSkinningBoneFrameCaches) delete[] m_ppSkinningBoneFrameCaches;
	if (m_ppstrSkinningBoneNames) delete[] m_ppstrSkinningBoneNames;

	if (m_pxmf4x4BindPoseBoneOffsets) delete[] m_pxmf4x4BindPoseBoneOffsets;
	if (m_pd3dcbBindPoseBoneOffsets) m_pd3dcbBindPoseBoneOffsets->Release();

	if (m_pd3dBoneIndexBuffer) m_pd3dBoneIndexBuffer->Release();
	if (m_pd3dBoneWeightBuffer) m_pd3dBoneWeightBuffer->Release();

	ReleaseShaderVariables();
}

void SkinnedMesh::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void SkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbBindPoseBoneOffsets)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbBindPoseBoneOffsets->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dcbBoneOffsetsGpuVirtualAddress); //Skinned Bone Offsets
	}

	if (m_pd3dcbSkinningBoneTransforms)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_pd3dcbSkinningBoneTransforms->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(12, d3dcbBoneTransformsGpuVirtualAddress); //Skinned Bone Transforms

		//애니메이터에서 처리..
		//for (int j = 0; j < m_nSkinningBones; j++)
		//{
		//	//XMStoreFloat4x4(&m_pcbxmf4x4MappedSkinningBoneTransforms[j], XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinningBoneFrameCaches[j]->m_xmf4x4World)));
		//}
	}
}

void SkinnedMesh::ReleaseShaderVariables()
{
}

void SkinnedMesh::ReleaseUploadBuffers()
{
	StandardMesh::ReleaseUploadBuffers();

	if (m_pd3dBoneIndexUploadBuffer) m_pd3dBoneIndexUploadBuffer->Release();
	m_pd3dBoneIndexUploadBuffer = NULL;

	if (m_pd3dBoneWeightUploadBuffer) m_pd3dBoneWeightUploadBuffer->Release();
	m_pd3dBoneWeightUploadBuffer = NULL;
}

void SkinnedMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}

}

void SkinnedMesh::PrepareSkinning(GameObject* pModelRootObject)
{

	m_ppSkinningBoneFrameCaches = new GameObject * [m_nSkinningBones];
	for (int j = 0; j < m_nSkinningBones; j++)
	{
		m_ppSkinningBoneFrameCaches[j] = pModelRootObject->FindFrame(m_ppstrSkinningBoneNames[j]);
	}

}

void SkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			m_nBonesPerVertex = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_ppstrSkinningBoneNames = new char[m_nSkinningBones][64];
				for (int i = 0; i < m_nSkinningBones; i++) ::ReadStringFromFile(pInFile, m_ppstrSkinningBoneNames[i]);
			}
		}

		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_pxmf4x4BindPoseBoneOffsets = new XMFLOAT4X4[m_nSkinningBones];
				nReads = (UINT)::fread(m_pxmf4x4BindPoseBoneOffsets, sizeof(XMFLOAT4X4), m_nSkinningBones, pInFile);

				UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
				m_pd3dcbBindPoseBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
				m_pd3dcbBindPoseBoneOffsets->Map(0, NULL, (void**)&m_pcbxmf4x4MappedBindPoseBoneOffsets);

				for (int i = 0; i < m_nSkinningBones; i++)
				{
					XMStoreFloat4x4(&m_pcbxmf4x4MappedBindPoseBoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4BindPoseBoneOffsets[i])));
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneIndices>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ::ReadIntegerFromFile(pInFile);
			if (m_nVertices > 0)
			{
				m_pxmn4BoneIndices = new XMINT4[m_nVertices];

				nReads = (UINT)::fread(m_pxmn4BoneIndices, sizeof(XMINT4), m_nVertices, pInFile);
				m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmn4BoneIndices, sizeof(XMINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);

				m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
				m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMINT4);
				m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMINT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ::ReadIntegerFromFile(pInFile);
			if (m_nVertices > 0)
			{
				m_pxmf4BoneWeights = new XMFLOAT4[m_nVertices];

				nReads = (UINT)::fread(m_pxmf4BoneWeights, sizeof(XMFLOAT4), m_nVertices, pInFile);
				m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4BoneWeights, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);

				m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
				m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}
	}
}

void SkinnedMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[7] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView, m_d3dBoneIndexBufferView, m_d3dBoneWeightBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 7, pVertexBufferViews);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									UIRectMesh Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
UIRectMesh::UIRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(XMFLOAT3);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, m_nStride * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dPositionBuffer->Map(0, NULL, (void**)&m_pcbMappedPositions);
	m_nVertexBufferViews = 1;
	m_d3dVertexBufferViews.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferViews.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexBufferViews.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;



	//Uv Coord
	XMFLOAT2 BasicUvPosition[6];
	// left top
	BasicUvPosition[0].x = 0.0f; BasicUvPosition[0].y = 0.0f;
	//right top
	BasicUvPosition[1].x = 1.0f; BasicUvPosition[1].y = 0.0f;
	//left bottom
	BasicUvPosition[2].x = 0.0f;	BasicUvPosition[2].y = 1.0f;

	//left bottom
	BasicUvPosition[3].x = 0.0f;	BasicUvPosition[3].y = 1.0f;
	//right top
	BasicUvPosition[4].x = 1.0f;	BasicUvPosition[4].y = 0.0f;
	//right bottom 
	BasicUvPosition[5].x = 1.0f;	BasicUvPosition[5].y = 1.0f;


	m_pd3dUvBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, BasicUvPosition, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dUvBuffer->Map(0, NULL, (void**)&m_pcbMappedUvs);
	m_d3dUvBufferViews.BufferLocation = m_pd3dUvBuffer->GetGPUVirtualAddress();
	m_d3dUvBufferViews.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dUvBufferViews.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;


	//Maskuv
	m_pd3dMaskUvBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, BasicUvPosition, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dMaskUvBuffer->Map(0, NULL, (void**)&m_pcbMappedMaskUvs);
	m_d3dMaskUvBufferViews.BufferLocation = m_pd3dMaskUvBuffer->GetGPUVirtualAddress();
	m_d3dMaskUvBufferViews.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dMaskUvBufferViews.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;



	//MaskCoord
	float BasicMaskPosition[6];
	BasicMaskPosition[0] = 0.0f;
	BasicMaskPosition[1] = 0.0f;
	BasicMaskPosition[2] = 0.0f;

	BasicMaskPosition[3] = 0.0f;
	BasicMaskPosition[4] = 0.0f;
	BasicMaskPosition[5] = 0.0f;

	m_pd3dMaskBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, BasicMaskPosition, sizeof(UINT) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dMaskBuffer->Map(0, NULL,(void**)&m_pcbMappedMasks);
	m_d3dMaskBufferViews.BufferLocation = m_pd3dMaskBuffer->GetGPUVirtualAddress();
	m_d3dMaskBufferViews.StrideInBytes = sizeof(UINT);
	m_d3dMaskBufferViews.SizeInBytes = sizeof(UINT) * m_nVertices;
}

UIRectMesh::~UIRectMesh()
{
	if (m_pd3dPositionBuffer) {
		m_pd3dPositionBuffer->Unmap(0, NULL);
		m_pd3dPositionBuffer->Release();
	}

	if (m_pd3dUvBuffer) {
		m_pd3dUvBuffer->Unmap(0, NULL);
		m_pd3dUvBuffer->Release();
	}

	if (m_pd3dMaskUvBuffer) {
		m_pd3dMaskUvBuffer->Unmap(0, NULL);
		m_pd3dMaskUvBuffer->Release();
	}

	if (m_pd3dMaskBuffer) {
		m_pd3dMaskBuffer->Unmap(0, NULL);
		m_pd3dMaskBuffer->Release();
	}
}

void UIRectMesh::UpdateVertexPosition(const UIRect& Rect)
{
	int i = 0;

	//LEFT TOP 
	m_pcbMappedPositions[i++] = XMFLOAT3(Rect.left, Rect.top, 0.5f);
	// RIGHT TOP
	m_pcbMappedPositions[i++] = XMFLOAT3(Rect.right, Rect.top, 0.5f);
	// LEFT BOTTOM
	m_pcbMappedPositions[i++] = XMFLOAT3(Rect.left, Rect.bottom, 0.5f);

	// LEFT BOTTOM
	m_pcbMappedPositions[i++] = XMFLOAT3(Rect.left, Rect.bottom, 0.5f);
	// RIGHT TOP
	m_pcbMappedPositions[i++] = XMFLOAT3(Rect.right, Rect.top, 0.5f);
	// RIGHT BOTTOM
	m_pcbMappedPositions[i++] = XMFLOAT3(Rect.right, Rect.bottom, 0.5f);

}

void UIRectMesh::UpdateUvCoord(const UIRect& Rect)
{

	// left top
	m_pcbMappedUvs[0].x = Rect.left; m_pcbMappedUvs[0].y = Rect.top;
	//right top
	m_pcbMappedUvs[1].x = Rect.right; m_pcbMappedUvs[1].y = Rect.top;
	//left bottom
	m_pcbMappedUvs[2].x = Rect.left; m_pcbMappedUvs[2].y = Rect.bottom;


	////left bottom
	m_pcbMappedUvs[3].x = Rect.left;  m_pcbMappedUvs[3].y = Rect.bottom;
	////right top
	m_pcbMappedUvs[4].x = Rect.right;	m_pcbMappedUvs[4].y = Rect.top;
	////right bottom 
	m_pcbMappedUvs[5].x = Rect.right;	m_pcbMappedUvs[5].y = Rect.bottom;
}

void UIRectMesh::UpdateMaskUvCoord(const UIRect& Rect)
{
	// left top
	m_pcbMappedMaskUvs[0].x = Rect.left; m_pcbMappedMaskUvs[0].y = Rect.top;
	//right top
	m_pcbMappedMaskUvs[1].x = Rect.right; m_pcbMappedMaskUvs[1].y = Rect.top;
	//left bottom
	m_pcbMappedMaskUvs[2].x = Rect.left; m_pcbMappedMaskUvs[2].y = Rect.bottom;


	////left bottom
	m_pcbMappedMaskUvs[3].x = Rect.left;  m_pcbMappedMaskUvs[3].y = Rect.bottom;
	////right top
	m_pcbMappedMaskUvs[4].x = Rect.right;	m_pcbMappedMaskUvs[4].y = Rect.top;
	////right bottom 
	m_pcbMappedMaskUvs[5].x = Rect.right;	m_pcbMappedMaskUvs[5].y = Rect.bottom;
}

void UIRectMesh::UpdateMaskValue(const UINT& lefttop, const UINT& righttop, const UINT& leftbottom, const UINT& rightbottom)
{
	
	m_pcbMappedMasks[0] = lefttop;
	m_pcbMappedMasks[1] = righttop;
	m_pcbMappedMasks[2] = leftbottom;

	m_pcbMappedMasks[3] = leftbottom;
	m_pcbMappedMasks[4] = righttop;
	m_pcbMappedMasks[5] = rightbottom;
}

void UIRectMesh::UpdateMaskValue(const UINT& AllRectMask)
{
	m_pcbMappedMasks[0] = AllRectMask;
	m_pcbMappedMasks[1] = AllRectMask;
	m_pcbMappedMasks[2] = AllRectMask;
	m_pcbMappedMasks[3] = AllRectMask;
	m_pcbMappedMasks[4] = AllRectMask;
	m_pcbMappedMasks[5] = AllRectMask;
}

void UIRectMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[4] = { m_d3dVertexBufferViews,m_d3dUvBufferViews,m_d3dMaskUvBufferViews ,m_d3dMaskBufferViews };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 4, pVertexBufferViews);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									AABBBoundingMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

BoundingBoxMesh::BoundingBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nVertices = 12 * 2;
	m_nStride = sizeof(XMFLOAT3);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, m_nStride * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dPositionBuffer->Map(0, NULL, (void**)&m_pcbMappedPositions);

	m_nVertexBufferViews = 1;

	m_d3dVertexBufferViews.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferViews.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexBufferViews.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}

BoundingBoxMesh::~BoundingBoxMesh()
{
	if (m_pd3dPositionBuffer) {
		m_pd3dPositionBuffer->Unmap(0, NULL);
		m_pd3dPositionBuffer->Release();
	}
}

void BoundingBoxMesh::UpdateVertexPosition(BoundingOrientedBox* pxmBoundingBox)
{
	XMFLOAT3 xmf3Corners[8];
	pxmBoundingBox->GetCorners(xmf3Corners);

	int i = 0;

	m_pcbMappedPositions[i++] = xmf3Corners[0];
	m_pcbMappedPositions[i++] = xmf3Corners[1];

	m_pcbMappedPositions[i++] = xmf3Corners[1];
	m_pcbMappedPositions[i++] = xmf3Corners[2];

	m_pcbMappedPositions[i++] = xmf3Corners[2];
	m_pcbMappedPositions[i++] = xmf3Corners[3];

	m_pcbMappedPositions[i++] = xmf3Corners[3];
	m_pcbMappedPositions[i++] = xmf3Corners[0];

	m_pcbMappedPositions[i++] = xmf3Corners[4];
	m_pcbMappedPositions[i++] = xmf3Corners[5];

	m_pcbMappedPositions[i++] = xmf3Corners[5];
	m_pcbMappedPositions[i++] = xmf3Corners[6];

	m_pcbMappedPositions[i++] = xmf3Corners[6];
	m_pcbMappedPositions[i++] = xmf3Corners[7];

	m_pcbMappedPositions[i++] = xmf3Corners[7];
	m_pcbMappedPositions[i++] = xmf3Corners[4];

	m_pcbMappedPositions[i++] = xmf3Corners[0];
	m_pcbMappedPositions[i++] = xmf3Corners[4];

	m_pcbMappedPositions[i++] = xmf3Corners[1];
	m_pcbMappedPositions[i++] = xmf3Corners[5];

	m_pcbMappedPositions[i++] = xmf3Corners[2];
	m_pcbMappedPositions[i++] = xmf3Corners[6];

	m_pcbMappedPositions[i++] = xmf3Corners[3];
	m_pcbMappedPositions[i++] = xmf3Corners[7];
}

void BoundingBoxMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_nVertexBufferViews, &m_d3dVertexBufferViews);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										SkyBoxMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

SkyBoxMesh::SkyBoxMesh(ID3D12Device* pd3dDeivce, ID3D12GraphicsCommandList* pd3dCommandList):BasicMesh(pd3dDeivce, pd3dCommandList){

	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	float length = 200.0f;

	float fWidth = length, fHeight = length, fDepth = length;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	// Front Quad (quads point inward)
	m_pxmf3Positions[0] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[1] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[2] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[3] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[4] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[5] = XMFLOAT3(+fx, -fx, +fx);
	// Back Quad										
	m_pxmf3Positions[6] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[7] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[8] = XMFLOAT3(+fx, -fx, -fx);
	m_pxmf3Positions[9] = XMFLOAT3(+fx, -fx, -fx);
	m_pxmf3Positions[10] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[11] = XMFLOAT3(-fx, -fx, -fx);
	// Left Quad										
	m_pxmf3Positions[12] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[13] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[14] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[15] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[16] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[17] = XMFLOAT3(-fx, -fx, +fx);
	// Right Quad										
	m_pxmf3Positions[18] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[19] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[20] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[21] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[22] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[23] = XMFLOAT3(+fx, -fx, -fx);
	// Top Quad											
	m_pxmf3Positions[24] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[25] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[26] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[27] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[28] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[29] = XMFLOAT3(+fx, +fx, +fx);
	// Bottom Quad										
	m_pxmf3Positions[30] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[31] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[32] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[33] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[34] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[35] = XMFLOAT3(+fx, -fx, -fx);

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDeivce, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}

SkyBoxMesh::~SkyBoxMesh() {}

ShphereMesh::ShphereMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nslice, int nstack ,float radian): 
	m_nSlices(nslice), 
	m_nStacks(nstack),
	m_fRadius(radian)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_nVertices = 2 + (m_nSlices * (m_nStacks - 1));
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	//180도를 nStacks 만큼 분할한다. 
	float fDeltaPhi = float(XM_PI / m_nStacks);
	//360도를 nSlices 만큼 분할한다. 
	float fDeltaTheta = float((2.0f * XM_PI) / m_nSlices);

	int k = 0;

	float theta_i, phi_j;

	m_pxmf3Positions[k++] = XMFLOAT3(0.0f, +m_fRadius, 0.0f);

	for (int j = 1; j < m_nStacks; ++j) {

		phi_j = fDeltaPhi * j;
		for (int i = 0; i < m_nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pxmf3Positions[k++] = XMFLOAT3(m_fRadius * sinf(phi_j) * cosf(theta_i),
				m_fRadius * cosf(phi_j), m_fRadius * sinf(phi_j) * sinf(theta_i));

			
		}

	}
	m_pxmf3Positions[k++] = XMFLOAT3(0.0f, -m_fRadius, 0.0f);

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions,
		sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_nSubMeshes = 1;

	int nIndices = (m_nSlices * 3) * 2 + (m_nSlices * (m_nStacks - 2) * 3 * 2);

	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];


	m_pnSubSetIndices[0] = nIndices;
	m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];
	k = 0;

	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < m_nSlices; i++)
	{
		m_ppnSubSetIndices[0][k++] = 0;
		m_ppnSubSetIndices[0][k++] = 1 + ((i + 1) % m_nSlices);
		m_ppnSubSetIndices[0][k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int j = 0; j < m_nStacks-2; j++)
	{
		for (int i = 0; i < m_nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다. 
			m_ppnSubSetIndices[0][k++] = 1 + (i + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % m_nSlices) + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (i + ((j + 1) * m_nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다.
			m_ppnSubSetIndices[0][k++] = 1 + (i + ((j + 1) * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % m_nSlices) + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % m_nSlices) + ((j + 1) * m_nSlices));
		}
	}


	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < m_nSlices; i++)
	{
		m_ppnSubSetIndices[0][k++] = (m_nVertices - 1);
		m_ppnSubSetIndices[0][k++] = ((m_nVertices - 1) - m_nSlices) + i;
		m_ppnSubSetIndices[0][k++] = ((m_nVertices - 1) - m_nSlices) + ((i + 1) % m_nSlices);
	}



	m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]);

	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];




}

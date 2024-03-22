#include "PerceptionMesh.h"


PerceptionRangeMesh::PerceptionRangeMesh(ID3D12Device* pd3dDeivce, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_nVertices = m_nSlices * 2;
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	//180도를 nStacks 만큼 분할한다. 
	float fDeltaPhi = float(XM_PI / m_nStacks);
//360도를 nSlices 만큼 분할한다. 
	float fDeltaTheta = float((2.0f * XM_PI) / m_nSlices);

	int k = 0;
	int MidStack = m_nStacks / 2;
	float theta_i, phi_j;

	for (int j = MidStack - 1; j < MidStack + 1; ++j) {

		phi_j = fDeltaPhi * j;
		for (int i = 0; i < m_nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pxmf3Positions[k] = XMFLOAT3(m_fRadius * sinf(phi_j) * cosf(theta_i),
				m_fRadius * cosf(phi_j), m_fRadius * sinf(phi_j) * sinf(theta_i));
			if (m_pxmf3Positions[k].x == 0 && m_pxmf3Positions[k].y == 0 && m_pxmf3Positions[k].z == 0) {
				break;
			}
			k++;
		}

	}

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDeivce, pd3dCommandList, m_pxmf3Positions,
		sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_nSubMeshes = 1;

	int nIndices = m_nSlices * 8 ;

	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];


	m_pnSubSetIndices[0] = nIndices;
	m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];
	k = 0;
	for (int j = 0; j < 1; j++)
	{
		for (int i = 0; i < m_nSlices ; i++)
		{


			m_ppnSubSetIndices[0][k++] = (i + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = (((i + 1) % m_nSlices) + (j * m_nSlices));

			m_ppnSubSetIndices[0][k++] = (((i + 1) % m_nSlices) + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = (((i + 1) % m_nSlices) + (j+1 * m_nSlices));

			m_ppnSubSetIndices[0][k++] = (((i + 1) % m_nSlices) + (j + 1 * m_nSlices));
			m_ppnSubSetIndices[0][k++] = (((i ) % m_nSlices) + (j + 1 * m_nSlices));

			m_ppnSubSetIndices[0][k++] = (((i) % m_nSlices) + (j + 1 * m_nSlices));
			m_ppnSubSetIndices[0][k++] = (i + (j * m_nSlices));
		}
	}

	m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(pd3dDeivce, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]);

	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];
	
	//FOV 범위 지정

	m_nFovVertices = 3 * 2;
	m_nStride = sizeof(XMFLOAT3);
	m_nOffset = 0;
	m_nSlot = 0;
	m_nVertexFOVBufferViews = 1;
	m_d3dFOVPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	m_pd3dFOVPositionBuffer = CreateBufferResource(pd3dDeivce, pd3dCommandList, NULL, m_nStride * m_nFovVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dFOVPositionBuffer->Map(0, NULL, (void**)&m_pcbMappedFOVPositions);

	m_d3dVertexFOVBufferViews.BufferLocation = m_pd3dFOVPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexFOVBufferViews.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexFOVBufferViews.SizeInBytes = sizeof(XMFLOAT3) * m_nFovVertices;
	
	int i = 0;
	
	XMFLOAT3 RealDir = XMFLOAT3(0.f, 0.f, 1.f);
	XMFLOAT3 rotatedVector;

	m_pcbMappedFOVPositions[i++] = XMFLOAT3(0.f, AIFOVHEIGHT, 0.f);
	
	RealDir = Vector3::ScalarProduct(RealDir, m_fRadius, true);
	RealDir.y = AIFOVHEIGHT;
	m_pcbMappedFOVPositions[i++] = RealDir;
	RealDir.y = 0.f;


	rotatedVector.x = RealDir.x * cos(m_fov) + RealDir.z * sin(m_fov);
	rotatedVector.y = RealDir.y;
	rotatedVector.z = -RealDir.x * sin(m_fov) + RealDir.z * cos(m_fov);
	rotatedVector = Vector3::ScalarProduct(rotatedVector, m_fRadius, true);
	m_pcbMappedFOVPositions[i++] = XMFLOAT3(0.f, AIFOVHEIGHT, 0.f);
	rotatedVector.y = AIFOVHEIGHT;
	m_pcbMappedFOVPositions[i++] = rotatedVector;
	

	rotatedVector.x = RealDir.x * cos(-m_fov) + RealDir.z * sin(-m_fov);
	rotatedVector.y = RealDir.y;
	rotatedVector.z = -RealDir.x * sin(-m_fov) + RealDir.z * cos(-m_fov);
	rotatedVector = Vector3::ScalarProduct(rotatedVector, m_fRadius, true);

	m_pcbMappedFOVPositions[i++] = XMFLOAT3(0.f, AIFOVHEIGHT, 0.f);
	rotatedVector.y = AIFOVHEIGHT;
	m_pcbMappedFOVPositions[i++] = rotatedVector;

}

PerceptionRangeMesh::~PerceptionRangeMesh()
{
	if (m_pd3dFOVPositionBuffer) {
		m_pd3dFOVPositionBuffer->Unmap(0, NULL);
		m_pd3dFOVPositionBuffer->Release();
	}

}

void PerceptionRangeMesh::PerceptionRangeRender(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	BasicMesh::Render(pd3dCommandList,0);


}
void PerceptionRangeMesh::PerceptionBindRender(ID3D12GraphicsCommandList* pd3dCommandList) {
	pd3dCommandList->IASetPrimitiveTopology(m_d3dFOVPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_nVertexFOVBufferViews, &m_d3dVertexFOVBufferViews);
	pd3dCommandList->DrawInstanced(m_nFovVertices, 1, m_nOffset, 0);
}
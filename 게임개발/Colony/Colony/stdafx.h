﻿#pragma once

#define FRAME_BUFFER_WIDTH  1920
#define FRAME_BUFFER_HEIGHT	1080
#define WINDOWS_POS_X	 0
#define WINDOWS_POS_Y	 0
//자주 사용하지 않는 API의 일부를 제외하는 명령어
#ifndef WIN32_LEAN_AND_MEAN //활성화 상태
# define WIN32_LEAN_AND_MEAN
#endif


//Light 정의
#define MAX_LIGHTS						3
#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT				3

#define MAX_DEPTH_TEXTURES		MAX_LIGHTS
#define _DEPTH_BUFFER_WIDTH		(FRAME_BUFFER_WIDTH * 4)
#define _DEPTH_BUFFER_HEIGHT	(FRAME_BUFFER_HEIGHT * 4)

#define _PLANE_WIDTH			512
#define _PLANE_HEIGHT			512

#define _WITH_RASTERIZER_DEPTH_BIAS

#include "targetver.h"


//다이렉트 디버그 옵션
#ifdef _DEBUG  
#include <dxgidebug.h>
#endif

//윈도우
#include <Windows.h>

//c 라이브러리
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <wrl.h>
#include <shellapi.h>
#include <cmath>

//c++ 라이브러리
#include <fstream>
#include <string>
// STL 관련 헤더파일들
#include <functional>
#include <string>
#include <deque>
#include <list>
#include <algorithm>
#include <vector>
#include <stack>
#include <random>
#include <thread>
#include <chrono>
#include <mutex>
#include <concurrent_queue.h>
////////////////////////////////

//Direct x 관련 라이브러리
#include <d3d12.h> 
#include <dxgi1_4.h>
#include <D3Dcompiler.h> //shader compiler
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
//#include <wrl/client.h>
#include <Mmsystem.h>
////Textrue관련
//#include "d3dx12.h"
//#include "DDSTextureLoader.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;
//사용자 정의 헤더파일
#include "UserDefineType.h"

//파일 입출력
extern BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken);
extern int ReadIntegerFromFile(FILE* pInFile);
extern float ReadFloatFromFile(FILE* pInFile);

//GPU 메모리 할당 함수
extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3dUploadBuffer = NULL);
extern ID3D12Resource* CreateTextureResourceFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
extern ID3D12Resource* CreateTextureResourceFromWICFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
extern ID3D12Resource* CreateTexture2DResource(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);
#define EPSILON					1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }
inline float XM2CalDis(const XMFLOAT2& a, const XMFLOAT2& b) { return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); }
inline float XM3CalDis(const XMFLOAT3& a, const XMFLOAT3& b) { return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z)); }
inline float VectorSize(XMFLOAT3 V) {	return (float)sqrt(V.x * V.x + V.y * V.y + V.z * V.z);}

inline bool RangeCheck(int min, int max, int CheckValue) {

	if (min <= CheckValue) {
		if (CheckValue <= max) {
			return true;
		}
	}
	return false;
}

inline float VectorLength(XMFLOAT3 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}
inline float DistancePointToPlane(XMFLOAT3 point, XMFLOAT3 planeNormal, XMFLOAT3 planePoint) {
	// XMFLOAT3에서 XMVECTOR로 변환
	XMVECTOR vecPoint = XMLoadFloat3(&point);
	XMVECTOR vecPlaneNormal = XMLoadFloat3(&planeNormal);
	XMVECTOR vecPlanePoint = XMLoadFloat3(&planePoint);

	// 법선 벡터 정규화
	vecPlaneNormal = XMVector3Normalize(vecPlaneNormal);

	// 점과 평면 사이의 거리 계산
	float numerator = XMVectorGetX(XMVector3Dot(vecPlaneNormal, vecPoint)) - XMVectorGetX(XMVector3Dot(vecPlaneNormal, vecPlanePoint));
	return fabs(numerator);  // 절대값 계산
}


//멀티쓰레드
#define MAX_THREAD_NUM	4
#define WITH_MULTITHREAD 

inline bool CAS(volatile int* addr, int expected, int new_val)
{
	return atomic_compare_exchange_strong(
		reinterpret_cast<volatile atomic_int*>(addr),
		&expected, new_val);
}

inline XMFLOAT3 RotateAroundYAxis(const XMFLOAT3& dir, float theta)
{
	// 각도를 라디안으로 변환
	float rad = XMConvertToRadians(theta);

	// 회전 행렬 생성
	XMMATRIX rotationMatrix = XMMatrixRotationY(rad);

	// dir 벡터를 XMVECTOR로 변환
	XMVECTOR dirVec = XMLoadFloat3(&dir);

	// 회전 행렬을 사용하여 벡터 회전
	XMVECTOR rotatedVec = XMVector3Transform(dirVec, rotationMatrix);

	// 결과를 XMFLOAT3로 변환
	XMFLOAT3 rotatedDir;
	XMStoreFloat3(&rotatedDir, rotatedVec);

	return rotatedDir;
}

void ResourceTransition(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dResource, D3D12_RESOURCE_STATES d3dStateBefore, D3D12_RESOURCE_STATES d3dStateAfter);
void SwapResourcePointer(ID3D12Resource** ppd3dResourceA, ID3D12Resource** ppd3dResourceB);

inline int ReadUnityBinaryString(FILE* pFile, char* pstrToken, BYTE* pnStrLength)
{
	UINT nReads = 0;
	nReads = (UINT)::fread(pnStrLength, sizeof(BYTE), 1, pFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), *pnStrLength, pFile);
	pstrToken[*pnStrLength] = '\0';

	return(nReads);
}

//루트시그너쳐 등록 숫자
enum RootSignatureNum{
	CAMERA_INFO,
	GAMEOBJECT_INFO,
	LIGHT_INFO,
	ALBEDO_TEXTURE,
	SPECULAR_TEXTURE,
	NORMAL_TEXTURE,
	METALLIC_TEXTURE,
	EMISSION_TEXTURE,
	DETAIL_ALBEDO_TEXTURE,
	DETAIL_NORMAL_TEXTURE,
	SKYBOX,
	SKINNED_BONE_OFFSET,
	SKINNED_BONE_TRANSFORM,
	UI_TEXTURE,
	UI_MASK_TEXTURE,
	DEPTH_TEXTURE,
	LIGHT_TEXTURE,
	BILLBOARD_TEXTURE,
	PARTICLE_TEXTURE
};

namespace DebugValue {

	inline void PrintVector3(const XMFLOAT3& v) {
		OutputDebugStringA("x: ");
		OutputDebugStringA(to_string(v.x).c_str());
		OutputDebugStringA("y: ");
		OutputDebugStringA(to_string(v.y).c_str());
		OutputDebugStringA("z: ");
		OutputDebugStringA(to_string(v.z).c_str());
		OutputDebugStringA("  \n ");
		
	}

	inline void Print4x4float(const char str[], const XMFLOAT4X4 m) {

		OutputDebugStringA(str);
		OutputDebugStringA("<행렬값> \n");
		OutputDebugStringA(to_string(m._11).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._12).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._13).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._14).c_str());
		OutputDebugStringA("  \n");

		OutputDebugStringA(to_string(m._21).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._22).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._23).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._24).c_str());
		OutputDebugStringA("  \n");

		OutputDebugStringA(to_string(m._31).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._32).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._33).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._34).c_str());
		OutputDebugStringA("  \n");

		OutputDebugStringA(to_string(m._41).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._42).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._43).c_str());
		OutputDebugStringA(" , ");
		OutputDebugStringA(to_string(m._44).c_str());
		OutputDebugStringA("  \n");


	}

	inline void PrintVector3(const char str[], const XMFLOAT3& v) {
		OutputDebugStringA(str);
		OutputDebugStringA("x: ");
		OutputDebugStringA(to_string(v.x).c_str());
		OutputDebugStringA("y: ");
		OutputDebugStringA(to_string(v.y).c_str());
		OutputDebugStringA("z: ");
		OutputDebugStringA(to_string(v.z).c_str());
		OutputDebugStringA("  \n ");

	}
	inline void Printfloat (const char str[], const float& f) {
		OutputDebugStringA(str);
		OutputDebugStringA("실수값: ");
		OutputDebugStringA(to_string(f).c_str());
		OutputDebugStringA("\n ");

	}
	inline void Printfloat (const float& f) {

		OutputDebugStringA("실수값: ");
		OutputDebugStringA(to_string(f).c_str());
		OutputDebugStringA("\n ");

	}
	inline void Printbool(const bool& b) {
		OutputDebugStringA(to_string(b).c_str());
	}

	inline void PrintStr(const char str[]) {
		OutputDebugStringA(str);
	}
}


//Direct 관련 함수들
namespace Vector3
{

	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}

	inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(XMVectorGetX(xmvAngle)));
	}

	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixIdentity());
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Zero()
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixMultiply(XMLoadFloat4x4(&xmmtx4x4Matrix1), XMLoadFloat4x4(&xmmtx4x4Matrix2)));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Scale(XMFLOAT4X4& xmf4x4Matrix, float fScale)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmf4x4Matrix) * fScale);
		/*
				XMVECTOR S, R, T;
				XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&xmf4x4Matrix));
				S = XMVectorScale(S, fScale);
				T = XMVectorScale(T, fScale);
				R = XMVectorScale(R, fScale);
				//R = XMQuaternionMultiply(R, XMVectorSet(0, 0, 0, fScale));
				XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
		*/
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Add(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) + XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Interpolate(XMFLOAT4X4& xmf4x4Matrix1, XMFLOAT4X4& xmf4x4Matrix2, float t)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMVECTOR S0, R0, T0, S1, R1, T1;
		XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&xmf4x4Matrix1));
		XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&xmf4x4Matrix2));
		XMVECTOR S = XMVectorLerp(S0, S1, t);
		XMVECTOR T = XMVectorLerp(T0, T1, t);
		XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmf4x4Result);
	}
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3& xmf3RayPosition, XMFLOAT3& xmf3RayDirection, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(XMLoadFloat3(&xmf3RayPosition), XMLoadFloat3(&xmf3RayDirection), XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fHitDistance));
	}
}

namespace Plane
{
	inline XMFLOAT4 Normalize(XMFLOAT4& xmf4Plane)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));
		return(xmf4Result);
	}
}
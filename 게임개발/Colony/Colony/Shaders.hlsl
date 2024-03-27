struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix					gmtxView : packoffset(c0);
	matrix					gmtxProjection : packoffset(c4);
	float3					gvCameraPosition : packoffset(c8);
    
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix					gmtxGameObject : packoffset(c0);
	MATERIAL				gMaterial : packoffset(c4);
	uint					gnTexturesMask : packoffset(c8.x);
    float                   gfGhostNum : packoffset(c8.y);
    float3                  BoundingColor : packoffset(c9);
};

#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);

SamplerState gssWrap : register(s0);

struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    
    float4 cAlbedoColor = gMaterial.m_cDiffuse;
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
    {
        float4 texColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
        cAlbedoColor.rgb *= texColor.rgb; 
        cAlbedoColor.a *= texColor.a; 
    }
    
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
    
    float4 cEmissionColor = gMaterial.m_cEmissive;
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
    {
        float4 texColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
        cEmissionColor.rgb *= texColor.rgb; 
        cEmissionColor.a *= texColor.a; 
    }
        float3 normalW;
        float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
        if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        {
            float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
            float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
            normalW = normalize(mul(vNormal, TBN));
        }
        else
        {
            normalW = normalize(input.normalW);
        }
	
        float4 cIllumination = Lighting(input.positionW, normalW);
        cColor = (lerp(cColor, cIllumination, 0.2f));

        return cColor;
    }

// AABBBouding 랜더링
struct VS_BOUNDINGBOX_INPUT
{
    float3 position : POSITION;
};

struct VS_BOUNDINGBOX_OUTPUT
{
    float4 positionH : SV_POSITION;
};

VS_BOUNDINGBOX_OUTPUT VSBoundingBox(VS_BOUNDINGBOX_INPUT input)
{
    VS_BOUNDINGBOX_OUTPUT output;
    output.positionH = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);

    return (output);
}

float4 PSBoundingBox(VS_BOUNDINGBOX_OUTPUT input) : SV_TARGET
{
    return (float4(BoundingColor, 1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128

cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;	
	
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
    {
        mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
    }

    output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
    output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
    output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
    output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;


	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float4 PSGhostTrailler(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
        cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

    float3 normalW;
    float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
    {
        float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
        normalW = normalize(mul(vNormal, TBN));
    }
    else
    {
        normalW = normalize(input.normalW);
    }
    //return cColor;
	
    float4 cIllumination = Lighting(input.positionW, normalW);
	//cColor = (lerp(cColor, cIllumination, 0.2f));
    //cColor.rgb *= 1;
    // cColor.r = 1-GhostTime;
    cColor.a = 0.7f - (gfGhostNum/100);
    cColor.rgb *= 0.7;
    cColor.rgb *= (1.0f - (gfGhostNum / 5));
    return cColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UI 텍스쳐 & 텍스쳐마스크
Texture2D gtxtUiTexture : register(t1);
Texture2D gtxtUiMaskTexture : register(t2);

struct VS_UIRECT_INPUT
{
    float3 position : POSITION;
    float2 TexC : TEXCOORD;
    float2 TexM : MASKTEXCOORD;
    uint Mask : MASK;
};


struct VS_UIRECT_OUTPUT
{
    float4 position : SV_POSITION;
    float2 TexC : TEXCOORD;
    float2 TexM : MASKTEXCOORD;
    uint Mask : MASK;
};

VS_UIRECT_OUTPUT VSUiRect(VS_UIRECT_INPUT input)
{
    VS_UIRECT_OUTPUT output;
    
    output.position = float4(input.position, 1.0f);
    output.TexC = input.TexC;
    output.TexM = input.TexM;
    output.Mask = input.Mask;
 
	
    return (output);
}
#define MASKUSE 0x01 //마스크 텍스쳐 사용 여부
#define TEXTUREUSE 0x02 // 텍스쳐 사용 여부
#define AMPLIFIER 0x04

float4 PSUiRect(VS_UIRECT_OUTPUT input) : SV_TARGET
{
    float4 texColor;
    
    if (input.Mask & MASKUSE)
    {
			
        if (input.Mask & TEXTUREUSE)
        {
            if (gtxtUiMaskTexture.Sample(gssWrap, input.TexM).r > 0.01)
            {
                texColor = float4(0, 0.1, 0.1, 1);
                texColor = gtxtUiTexture.Sample(gssWrap, input.TexC);
                texColor *= 1.6f;
               
            }
            else
            {
                texColor = gtxtUiTexture.Sample(gssWrap, input.TexC);

            }
			
        }
		

    }
    else
    {
        texColor = gtxtUiTexture.Sample(gssWrap, input.TexC);
    }

    return texColor;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

TextureCube gtxtSkyCubeTexture : register(t13);
SamplerState gssClamp : register(s1);

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

	return(cColor);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VS_STANDARD_OUTPUT VSPlane(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PSPlane(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    const float RepeatCount = 100.0f;
    input.uv.x *= RepeatCount;
    input.uv.y *= RepeatCount;
    
    float4 cAlbedoColor = gMaterial.m_cDiffuse;
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
    {
        float4 texColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
        cAlbedoColor.rgb *= texColor.rgb;
        cAlbedoColor.a *= texColor.a;
    }
    
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
    
    float4 cEmissionColor = gMaterial.m_cEmissive;
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
    {
        float4 texColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
        cEmissionColor.rgb *= texColor.rgb;
        cEmissionColor.a *= texColor.a;
    }
    float3 normalW;
    float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
    {
        float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
        normalW = normalize(mul(vNormal, TBN));
    }
    else
    {
        normalW = normalize(input.normalW);
    }
	
    float4 cIllumination = Lighting(input.positionW, normalW);
    //cColor = (lerp(cColor, cIllumination, 0.5f));

    return cColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NevMesh
struct VS_NEVMESH_INPUT
{
    float3 position : POSITION;
    uint use : USE;
};

struct PS_NEVMESH_OUTPUT
{
    float4 positionH : SV_POSITION;
    uint use : USE;
};

PS_NEVMESH_OUTPUT VSNevMesh(VS_NEVMESH_INPUT input)
{
    
    PS_NEVMESH_OUTPUT output;
    output.positionH = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.use = input.use;
    
    return output;
}

float4 PSNevMesh(PS_NEVMESH_OUTPUT input) : SV_TARGET
{
    
    float4 output;
    if (input.use)
    {
        output = float4(0.0f, 0.7f, 0.0f, 1.0f);
        
    }
    else
    {
        output = float4(0.7f, 0.0f, 0.0f, 1.0f);
    }
    return output;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DepthWriter
struct PS_DEPTH_OUTPUT
{
    float fzPosition : SV_TARGET0 ;
    float fDepth : SV_Depth;
};

PS_DEPTH_OUTPUT PSDepthWriteShader(VS_STANDARD_OUTPUT input) 
{
    PS_DEPTH_OUTPUT output;
    
    float4 texColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    output.fzPosition = texColor;
    output.fDepth.r = input.position.z;

    return (output);
}
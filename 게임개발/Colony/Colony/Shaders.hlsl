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
    matrix                  gmtxInverseView : packoffset(c8);
    float3                  gvCameraPosition : packoffset(c12);
    
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix					gmtxGameObject : packoffset(c0);
	MATERIAL				gMaterial : packoffset(c4);
	uint					gnTexturesMask : packoffset(c8.x);
    float                   gfGhostNum : packoffset(c8.y);
    float3                  BoundingColor : packoffset(c9);
    int                     PlayerVelocityScalar : packoffset(c9.w);
};

struct CB_TOOBJECTSPACE
{
    matrix mtxToTexture;
    float4 f4Position;
};
#include "Light.hlsl"

cbuffer cbToLightSpace : register(b3)
{
    CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING
#define DOWNGRAY 1.0f
#define LIGHTRATIO 1.0f
#define SCENE_SIZE_Dx     1.0f/1024.f
#define SCENE_SIZE_Dy     1.0f/800.f

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

#define gammaCorrection  2.2f

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
    
    float4 uvs[MAX_LIGHTS] : TEXCOORD1;
};
//Extend ReinHard
float luminance(float3 v)
{
    return dot(v, float3(0.2126f, 0.7152f, 0.0722f));
}
float3 chagne_luminance(float3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
    
}
float3 reinhard_extended_luminance(float3 v, float max_white_l)
{
    float l_old = luminance(v);
    float numberator = l_old * (1.0f + (l_old) / (max_white_l * max_white_l));
    float l_new = numberator / (1.0f + l_old);
    return chagne_luminance(v, l_new);
}

//Filmic TMO
float3 uncharted2_tonmap_partial(float3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;

}

float3 uncharted2_filmic(float3 v)
{
    float3 exposure_bias = float3(2.0,2.0,2.0);
    float3 curr = uncharted2_tonmap_partial(v * exposure_bias);
    
    float3 W = float3(11.2,11.2,11.2);
    float3 white_scale = float3(1.0,1.0f,1.0f) / uncharted2_tonmap_partial(W);
    return curr * white_scale;
}

static const float3x3 aces_input_matrix =
{
   0.59719f,0.35458,0.04823,
    0.07600,0.90834,0.01566,
    0.02840,0.13383 ,0.83777
};

static const float3x3 aces_ouput_matrix =
{
    1.60475,-0.53108f,-0.07367,
    -0.10208,1.10813,0.00605,
    -0.00327 , -0.07276,1.07602
 
};
float3 newmul(float3x3 m, float3 v)
{
    float x = m._11 * v.r + m._12 * v.b + m._13 * v.b;
    float y = m._21 * v.r + m._22 * v.b + m._23 * v.b;
    float z = m._31 * v.r + m._32 * v.b + m._33 * v.b;
    
    return float3(x, y, z);
}

float3 rtt_and_odt_fir(float3 v)
{
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
    
}

float3 aces_fitted(float3 v)
{
    v = mul(aces_input_matrix, v);
    v = rtt_and_odt_fir(v);
    return mul(aces_ouput_matrix, v);

}

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;
    float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.positionW = positionW.xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
    
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    
    float4 cAlbedoColor = gMaterial.m_cDiffuse;
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor *= pow(gtxtAlbedoTexture.Sample(gssWrap, input.uv), gammaCorrection);

    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = pow(gtxtSpecularTexture.Sample(gssWrap, input.uv), gammaCorrection);
    
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = pow(gtxtMetallicTexture.Sample(gssWrap, input.uv), gammaCorrection);
    
    float4 cEmissionColor = gMaterial.m_cEmissive;
    if(gnTexturesMask & MATERIAL_EMISSION_MAP) 
        cEmissionColor *= pow(gtxtEmissionTexture.Sample(gssWrap, input.uv), gammaCorrection);
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

    
    float4 cIllumination = Lighting(input.positionW, normalW, true, input.uvs);
    
    cColor.rgb = pow(cColor.rgb, 1 / gammaCorrection);

    cColor.rgb = reinhard_extended_luminance(cColor.rgb * cIllumination.rgb,1.0f);

    cColor.rgb *= cIllumination ;

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

    float4 positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);
    
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
    output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
    output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;


	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

    for (int j = 0; j < MAX_LIGHTS; j++)
    {
        if (gcbToLightSpaces[j].f4Position.w != 0.0f)
            output.uvs[j] = mul(positionW, gcbToLightSpaces[j].mtxToTexture);
    }
    
    
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
    float4 cIllumination = GhostLighting(input.positionW, normalW, false, input.uvs);
    
    //cColor = lerp(cColor, cIllumination, LIGHTRATIO);
   //cColor += cIllumination * LIGHTRATIO;

    cColor *= cIllumination * LIGHTRATIO ;
    
    float2 newUV = input.uv * 70;
    newUV.x += gfGhostNum;
    float4 mask = gtxtDetailNormalTexture.Sample(gssWrap, newUV);
    float timescale = 4;
    float uvalue = 0.5f * (1 - gfGhostNum * timescale) + 0.22f * (1 - gfGhostNum * timescale);
    float dvalue = 0.5f * (1 - gfGhostNum * timescale);
    
    

        
        if (mask.r < uvalue)
        {
            if (mask.r > dvalue)
            {
           
                cColor.a = 1.0f - gfGhostNum * timescale;

                cColor.rgb *= (0.3f * (1 - gfGhostNum * timescale) + 0.05f);
                cColor.rgb += 0.10f;
                return cColor;
            }
        }
    

       discard;
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
#define SELECTUI 0x08

float4 Blur(int size, float2 uv)
{
    int intensity = 0;
    float4 texColor = gtxtUiTexture.Sample(gssWrap, uv);
    if (size == 0)
    {
        if (intensity != 0)
        {
        
            int size = intensity;
        
            for (int j = -size; j < size; ++j)
            {
            
                for (int i = -size; i < size; ++i)
                {
                    if (i == 0 && j == 0)
                    {
                        continue;
                    }
                        
                    texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
                }
            }
            texColor.rgb /= float(2 * 2 * size * size);
        }
    }
    else if (size == 1 || size == 2 || size == 3)
    {
        int size = 1 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    else if (size == 1 || size == 2 || size == 3 || size == 4)
    {
        int size = 2 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    else if (size == 5 || size == 6 || size == 7 || size == 8)
    {
        int size = 3 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    else if (size == 9 || size == 10 || size == 11 || size == 12)
    {
        int size = 4 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    else if (size == 13 || size == 14 || size == 15 || size == 16)
    {
        int size = 5 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    else if (size == 17 || size == 18 || size == 19 || size == 20)
    {
        int size = 6 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    else
    {
        int size = 7 + intensity;
        
        for (int j = -size; j < size; ++j)
        {
            
            for (int i = -size; i < size; ++i)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                        
                texColor += gtxtUiTexture.Sample(gssWrap, uv + float2(i * SCENE_SIZE_Dx, j * SCENE_SIZE_Dy));
  
            }
        }
        texColor.rgb /= float(2 * 2 * size * size);
    }
    return texColor;
}

float4 PSUiRect(VS_UIRECT_OUTPUT input) : SV_TARGET
{

    float4 texColor = 0 ;
    
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
        if (input.Mask & AMPLIFIER)
        {
            
            texColor =  gtxtUiTexture.Sample(gssWrap, input.TexC);
            int size = PlayerVelocityScalar;
            if (size > 5)
            {
                float dis = (input.TexC.x
                -0.5f) * (input.TexC.x - 0.5f)
                +(input.TexC.y - 0.5f) * (input.TexC.y - 0.5f);
                
                dis *= 100;
                int uvsize = dis;
                texColor = Blur(uvsize, input.TexC);
                
            }
            
        }
        else
        {
        
            texColor = gtxtUiTexture.Sample(gssWrap, input.TexC);
        }
    }
    if (input.Mask & SELECTUI)
    {
        texColor.r+= 0.2f;
        texColor.g+= 0.2f;
        texColor.b+= 1.0f;
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
    cColor.xyz *= 0.6;
	return(cColor);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VS_STANDARD_OUTPUT VSPlane(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    float4 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.positionW = positionW.xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (gcbToLightSpaces[i].f4Position.w != 0.0f)
            output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
    }
    
    return (output);
}

float4 PSPlane(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    const float RepeatCount = 100.0f;
    input.uv.x *= RepeatCount;
    input.uv.y *= RepeatCount;

      
    float4 cAlbedoColor = gMaterial.m_cDiffuse;
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor *= pow(gtxtAlbedoTexture.Sample(gssWrap, input.uv), gammaCorrection);

    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = pow(gtxtSpecularTexture.Sample(gssWrap, input.uv), gammaCorrection);
    
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = pow(gtxtNormalTexture.Sample(gssWrap, input.uv), gammaCorrection);
    
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = pow(gtxtMetallicTexture.Sample(gssWrap, input.uv), gammaCorrection);
    
    float4 cEmissionColor = gMaterial.m_cEmissive;
    if (gnTexturesMask & MATERIAL_EMISSION_MAP) 
        cEmissionColor *= pow(gtxtEmissionTexture.Sample(gssWrap, input.uv), gammaCorrection);
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

    
    float4 cIllumination = Lighting(input.positionW, normalW, true, input.uvs);
    cColor.rgb = pow(cColor.rgb, 1 / gammaCorrection);
    

    cColor.rgb = reinhard_extended_luminance(cColor.rgb * cIllumination.rgb, 1.0f);
    cColor.rgb *= cIllumination;
    
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
    
   
    output.fzPosition = input.position.z;
    output.fDepth.r = input.position.z;

    return (output);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Billboard

Texture2D BillboardTexture : register(t20);

struct VS_BILLBOARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    
};

struct VS_BILLBOARD_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


VS_BILLBOARD_OUTPUT VSBILLBOARD(VS_BILLBOARD_INPUT input)
{
    VS_BILLBOARD_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}


float4 PSBILLBOARD(VS_BILLBOARD_OUTPUT input) : SV_TARGET
{
    float4 cColor = pow(BillboardTexture.Sample(gssWrap, input.uv), gammaCorrection);

    return (cColor);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Particle StreamOutput
struct VS_PARTICLE_INPUT
{
    float3 position : POSITION;
    uint type : TYPE;
    float3 direction : DIRECTION;
    float3 acceleration : ACC;
    float speed : SPEED;
    float lifeTime : LIFETIME;
    float age : AGE;
    float startTime : STARTTIME;
};

VS_PARTICLE_INPUT VSParticleStreamOutput(VS_PARTICLE_INPUT input)
{
    return (input);
}



[maxvertexcount(1)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
    VS_PARTICLE_INPUT particle = input[0];
    float startPos = 40.0;
    float age = frac(particle.age / particle.lifeTime) * particle.lifeTime;
    particle.position.y = startPos + age * particle.direction.y * particle.speed;
    particle.age += 0.1f /*gfElapsedTime*/;
    output.Append(particle);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Particle Draw 
struct VS_PARTICLE_OUTPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float size : SIZE;
};

struct GS_PARTICLE_DRAW_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXTURE;
};

VS_PARTICLE_OUTPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
    VS_PARTICLE_OUTPUT output = (VS_PARTICLE_OUTPUT) 0;

    output.position = input.position;
    output.size = 1.5f;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
    return (output);
}

static float3 gf3Positions[4] = { float3(-1.0f, +1.0f, 0.0f), float3(+1.0f, +1.0f, 0.0f), float3(-1.0f, -1.0f, 0.0f), float3(+1.0f, -1.0f, 0.0f) };
static float2 gf2QuadUVs[4] = { float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f) };

[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_OUTPUT input[1], inout TriangleStream<GS_PARTICLE_DRAW_OUTPUT> outputStream)
{
    GS_PARTICLE_DRAW_OUTPUT output = (GS_PARTICLE_DRAW_OUTPUT) 0;

    output.color = input[0].color;

    // Calculate billboard matrix
    float3 particlePos = input[0].position;
    float3 cameraPos = gmtxView[3].xyz; // Extract camera position from view matrix
    float3 look = normalize(cameraPos - particlePos);
    float3 right = normalize(cross(float3(0, 1, 0), look)); // Assume Y-axis is up
    float3 up = cross(look, right);

    // Construct billboard matrix
    float4x4 billboardMatrix = float4x4(
        right.x, right.y, right.z, 0,
        up.x, up.y, up.z, 0,
        look.x, look.y, look.z, 0,
        particlePos.x, particlePos.y, particlePos.z, 1
    );

    for (int i = 0; i < 4; i++)
    {
        float3 positionW = mul(gf3Positions[i] * input[0].size, (float3x3) gmtxInverseView) + input[0].position;
        output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
        output.uv = gf2QuadUVs[i];
        
        outputStream.Append(output);
    }
}


Texture2D ParticleTexture : register(t21);
float4 PSParticleDraw(GS_PARTICLE_DRAW_OUTPUT input) : SV_TARGET
{
    float4 cColor = ParticleTexture.Sample(gssWrap, input.uv);
    cColor *= input.color;
    

    return cColor;
}


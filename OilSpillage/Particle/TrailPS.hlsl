#include "..//ShaderDefines.hlsli"

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 wPos : WORLDPOS;
    float4 shadowPos : SHADOWPOS;
    float4 color : COLOR;
    float2 uv : UV;
};

struct Light
{
    float4 pos;
    float4 color;
    float4 directionWidth;
};

struct TileData
{
    uint numLights;
    uint indices[MAX_LIGHTS_PER_TILE];
};


cbuffer Lights : register(b1)
{
    Light lights[MAX_LIGHTS_TOTAL];
}

cbuffer SunInfo : register(b2)
{
    float4 sunColor;
    float4 sunDir;
};

cbuffer SpotLightIndex : register(b3)
{
    uint indexForSpot;
};
cbuffer CameraInfo : register(b4)
{
    float4 cameraPos;
}
Texture2D ShadowMap : register(t1);
SamplerState Sampler : register(s0);
SamplerState ShadowSamp : register(s1);
Texture2D Tex : register(t0);
StructuredBuffer<TileData> tileData : register(t2);
float shadowVisible(float4 shadowPosition, Texture2D shadowMap, float bias)
{
    float4 shadowCoord = shadowPosition;
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.xy = shadowCoord.xy * float2(0.5f, -0.5f) + 0.5f;
    float visibility = 0.0f;
    float4 pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(0, 0)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

    pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(-2, -0)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

    pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(-2, 0)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

    pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(0, -2)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;
	
    visibility *= 0.25 * 0.25;

    return visibility;
};

float4 main(PSInput input) : SV_Target
{
    float4 texColor = Tex.Sample(Sampler, input.uv).xyzw;
    float3 normal = float3(0, 1, 0);
    //float3 tangent = input.TangentWS.xyz;
    //float3 bitangent = input.BitangentWS.xyz;
    //float3 normalMap = NormalMap.Sample(SampSt, input.Tex).xyz;
    float4 specularColor = float4(0.0f,0.0f,0.0f,1.0f);
    float gloss = 0;
    //gloss = exp2(10 * gloss + 1);

    //if (length(normalMap) > 0.f)
    //{
    //    normalMap = 2.0f * normalMap - 1.0f;
    //    float3x3 TBN = float3x3(tangent, bitangent, normal);
    //    normal = normalize(mul(normalMap, TBN));
    //}

    uint2 lightTileIndex = uint2(input.pos.x * 0.0625f, input.pos.y * 0.0625f);
    TileData lightTileData = tileData[lightTileIndex.y * 80 + lightTileIndex.x];
    float sunShadow = (1 - shadowVisible(input.shadowPos, ShadowMap, 0.00015f));
    float4 ambient = max(-dot(sunDir.xyz, normal) * sunShadow, float4(0.2f, 0.2f, 0.2f, 1.0)) * sunColor;

    float shadowSpotVisible = 1.0f;

    float4 diffuseLight = float4(0.0, 0.0, 0.0, 1.0);
    float4 specularLight = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < lightTileData.numLights; ++i)
    {
        Light l = lights[lightTileData.indices[i]];
        float3 lightVector = l.pos.xyz - input.wPos.xyz;
        float attenuation;
        float nDotL = max(dot(normal, normalize(lightVector)), 0.0);
        float directional = 1.0;
        shadowSpotVisible = 1.0f;
		//if the light is a spot light
        if (l.pos.w == 1.0)
        {
            directional = 0.0;
            float s = dot(-normalize(lightVector), l.directionWidth.xyz);
            float umbra = cos(l.directionWidth.w);
            if (s > umbra)
            {
                float penumbra = cos(l.directionWidth.w * 0.9);
                directional = (s - umbra) / (penumbra - umbra);
                directional *= directional;
            }
            if (lightTileData.indices[i] == indexForSpot)
            {
                //shadowSpotVisible = shadowVisible(input.shadowPosSpot, ShadowMapSpot, 0.005f);
                //shadowSpotVisible = 1 - shadowSpotVisible * texColor.a;
            }
        }
        if (l.pos.w == 2.0)
        {
            float3 rayPointVector = (l.pos.xyz - input.wPos.xyz) - dot(l.pos.xyz - input.wPos.xyz, l.directionWidth.xyz) * l.directionWidth.xyz;
            float dist = dot(rayPointVector, rayPointVector);
			
            float startDist = dot(lightVector, lightVector);
            float3 endVector = (l.pos.xyz + l.directionWidth.xyz * l.directionWidth.w) - input.wPos.xyz;
            float endDist = dot(endVector, endVector);
			
            if (startDist < l.directionWidth.w * l.directionWidth.w && endDist < l.directionWidth.w * l.directionWidth.w)
            {
                attenuation = l.color.w / (dist * dist);
                nDotL = max(dot(normal, normalize(rayPointVector)), 0.0);
            }
            else
            {
                attenuation = l.color.w / (min(startDist, endDist) * min(startDist, endDist));
                if (startDist < endDist)
                {
                    nDotL = max(dot(normal, normalize(lightVector)), 0.0);
                }
                else
                {
                    nDotL = max(dot(normal, normalize(endVector.xyz)), 0.0);
                }
            }
        }
        else
        {
            attenuation = l.color.w / dot(lightVector, lightVector);
            nDotL = max(dot(normal, normalize(lightVector)), 0.0);
        }

        diffuseLight.rgb += max(l.color.rgb * nDotL * attenuation * directional * shadowSpotVisible, 0.0);
		
        float3 cameraVector = normalize(cameraPos.xyz - input.wPos.xyz);
        float3 halfway = normalize(normalize(lightVector) + cameraVector);
        specularLight.rgb += l.color.rgb * pow(max(dot(normal, halfway), 0.0), gloss) * attenuation * directional * shadowSpotVisible;
    }
    float4 setColor = input.color;
    setColor.w = 0.0f;
    float4 outColor = (texColor ) * (diffuseLight + ambient*0.1f);
    //outColor += (specularColor ) * specularLight;
    return outColor;
    //float4 texColor = Texture.Sample(Sampler, input.uv);
   // float4 finCol = float4(texColor.xyz,texColor.w);
   // return float4((1 - shadowVisible(input.shadowPos, ShadowMap, 0.00015f)) * finCol.xyz, finCol.w);
}
#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);  	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      	// 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	float3 normalLight = normalize(light);

	float lightPower = 0.75f;

	// 光源へのベクトルと法線ベクトルの内積
	float diffuse = saturate(dot(-normalLight, input.normal)) * lightPower;
	float ambient = 0.3f;
	float brightness = saturate(diffuse + ambient);
	if (brightness < 0.4f) brightness = 0.4;
	else if (brightness < 0.7f) brightness = 0.7f;
	else brightness = 0.8f;
	// テクスチャとシェーディングによる色を合成
	return float4(texcolor.rgb * brightness, texcolor.a) * color;
}

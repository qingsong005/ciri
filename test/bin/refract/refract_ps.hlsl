Texture3D CubemapTexture : register(t0);
TextureCube CubemapTextureSkybox : register(t1);
SamplerState CubemapSampler : register(s0);

struct Input {
	float4 Pos : SV_POSITION;
	float3 position : TEXCOORD1;
	float3 normal : NORMAL0;
	float2 Tex : TEXCOORD0;
	float3 ViewDir : TEXCOORD2;
};

static float3 AmbientLightColor = float3(0.05333332f, 0.09882354f, 0.1819608f);
static float3 LightDirection = float3(-0.5265408f, -0.5735765f,-0.6275069f);
static float3 LightColor = float3(1.0f, 0.9607844f, 0.8078432f);

static float4 DiffuseColor = float4(1.0, 1.0, 1.0, 1.0);
static float4 AmbientColor = float4(0.5, 0.5, 0.5, 1.0);



float3 lambert( float3 L, float3 N, float3 lightColor, float lightIntensity ) {
	return (max(dot(L, N), 0.0f) * lightColor) * lightIntensity;
}

float random( float2 p ) {
	float2 r = float2(23.14069263277926, 2.665144142690225);
	return frac(cos(dot(p, r)) * 123456.0);
}

float random2( float2 uv ) {
	float2 noise = (frac(sin(dot(uv ,float2(12.9898,78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

float3 RotateNormal( float angx, float angy, float3 N ){
	float cx = cos(angx);
	float sx = sin(angx);
	float cy = cos(angy);
	float sy = sin(angy);

	// rotate about x
	float yp = N.y*cx - N.z*sx; // y'
	N.z = N.y*sx + N.z*cx; // z'
	N.y = yp;

	// rotate about y
	float xp = N.x*cy + N.z*sy; // x'
	N.z = -N.x*sy + N.z*cy; // z'
	N.x = xp;

	return normalize(N);
}

float4 main( Input input ) : SV_Target {

	// can move into VS:
	float FresnelBias = 0.0;
	float FresnelScale = 0.42;
	float FresnelPow = 0.75;
	float3 EtaRatio = float3(0.66, 0.5, 0.57);
	//
	float3 L = normalize(-LightDirection);
	float3 N = normalize(input.normal);
	float3 V = normalize(input.ViewDir);
	//
	float3 R = reflect(V, N);
	float3 TRed = refract(V, N, EtaRatio.r);
	float3 TGreen = refract(V, N, EtaRatio.g);
	float3 TBlue = refract(V, N, EtaRatio.b);
	//
	float reflectionFactor = FresnelBias + FresnelScale * pow(1.0 + dot(V, N), FresnelPow);

	// can stay in ps
	float4 reflectedColor = CubemapTextureSkybox.Sample(CubemapSampler, R);
	float4 refractedColor;
	refractedColor.r = CubemapTextureSkybox.Sample(CubemapSampler, TRed).r;
	refractedColor.g = CubemapTextureSkybox.Sample(CubemapSampler, TGreen).g;
	refractedColor.b = CubemapTextureSkybox.Sample(CubemapSampler, TBlue).b;
	refractedColor.a = 1.0;
	float4 final = lerp(refractedColor, reflectedColor, reflectionFactor);
	// final.a = reflectionFactor;
	return final;
}

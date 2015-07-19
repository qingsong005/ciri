struct Input {
	float4 Pos : SV_POSITION;
	float3 Col : COLOR0;
};

float4 main( Input input ) : SV_Target
{
	return float4(input.Col, 1.0f);
}

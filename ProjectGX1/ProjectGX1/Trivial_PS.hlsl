float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET
{
		//colorFromRasterizer.y += 0.5f;

	return colorFromRasterizer;
}
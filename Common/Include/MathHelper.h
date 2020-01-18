#pragma once
#include <DirectXMath.h>

namespace MathHelper
{
	template <typename T>
	inline T Clamp(T value, T min, T max)
	{
		if (value < min) value = min;
		else if (value > max) value = max;
		return value;
	}

	inline DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 Identity = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		return Identity;
	}

	static float PI = 3.141592654f;
}
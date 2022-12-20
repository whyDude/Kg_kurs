#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <cstdint>

namespace config
{
	constexpr int16_t mainWindowWidth = 1100;
	constexpr int16_t mainWindowHeight = 650;

	namespace color
	{
		constexpr uint32_t background = 0xC4B8B8;
		constexpr uint32_t areaBackground = 0xC8C8C8; //0x2F4F4F;
		constexpr uint32_t areaBounds = 0xE82929;
	}

	namespace objectPreset
	{
		constexpr float lightPositionX = mainWindowWidth / 2;
		constexpr float lightPositionY = -500.0f;
		constexpr float lightPositionZ = 1.0f;

		constexpr float standartObject1PositionX = -150.0f;
		constexpr float standartObject1PositionY = 0.0f;
		constexpr float standartObject1PositionZ = 15.0f;

		constexpr float standartObject2PositionX = 150.0f;
		constexpr float standartObject2PositionY = 0.0f;
		constexpr float standartObject2PositionZ = 15.0f;

		constexpr float standartRotationX = 10.0f;
		constexpr float standartRotationY = 10.0f;
		constexpr float standartRotationZ = 10.0f;

		constexpr float standartScalingX = 0.2f;
		constexpr float standartScalingY = 0.2f;
		constexpr float standartScalingZ = 0.2f;

		namespace color
		{
			constexpr uint32_t standartColorBlue = 0x191970;
			constexpr uint32_t standartColorGreen = 0x228B22;
			constexpr uint32_t standartColorRed = 0x8B0000;

			constexpr uint32_t standartColorObject1 = 0xFF7C3E;
			constexpr uint32_t standartColorObject2 = 0xFFCF40;

			constexpr uint32_t standartEdgesColor = 0xF0FFF0;
			constexpr uint32_t standartShadowColor = 0x142233; //0x323232;
		}
	}

}

#endif // !_CONFIG_H_
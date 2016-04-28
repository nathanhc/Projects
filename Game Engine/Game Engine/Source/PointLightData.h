#pragma once
#include "Math.h"

const size_t MAX_POINT_LIGHTS = 8;

struct PointLightData
{
	PointLightData();

	Vector3 diffuseColor;
	float padding1;
	
	Vector3 specularColor;
	float padding2;
	
	Vector3 position;
	float specPower;
	
	float innerRad;
	float outerRad;
	unsigned int enabled;
	float padding3;
};

#pragma once
#include "Component.h"
#include "PointLightData.h"

class PointLightComponent : public Component
{
	DECL_COMPONENT(PointLightComponent, Component);
public:
	PointLightComponent(Actor& owner);

	void Register() override;
	void Unregister() override;

	void OnUpdatedTransform() override;

	const PointLightData& GetData() const { return mData; }

	// Add getter/setter functions for PointLightData
	Vector3 GetDiffuseColor() { return mData.diffuseColor; }
	void SetDiffuseColor(Vector3 color) { mData.diffuseColor = color; }

	Vector3 GetSpecularColor() { return mData.specularColor; }
	void SetSpecularColor(Vector3 color) { mData.specularColor = color; }

	Vector3 GetPosition() { return mData.position; }
	void SetPosition(Vector3 pos) { mData.position = pos; }

	float GetSpecularPower() { return mData.specPower; }
	void SetSpecularPower(float power) { mData.specPower = power; }

	float GetInnerRadius() { return mData.innerRad; }
	void SetInnerRadius(float rad) { mData.innerRad = rad; }

	float GetOuterRadius() { return mData.outerRad; }
	void SetOuterRadius(float rad) { mData.outerRad = rad; }

	unsigned int GetEnabled() { return mData.enabled; }
	void SetEnabled(unsigned int en) { mData.enabled = en; }

	void SetProperties(const rapidjson::Value& properties) override;
private:
	PointLightData mData;
};

DECL_PTR(PointLightComponent);

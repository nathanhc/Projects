#include "ITPEnginePCH.h"

IMPL_COMPONENT(PointLightComponent, Component, MAX_POINT_LIGHTS);

PointLightComponent::PointLightComponent(Actor& owner)
	:Component(owner)
{
	SetPosition(owner.GetPosition());
	SetEnabled(1);
}

void PointLightComponent::Register()
{
	Super::Register();
	mOwner.GetGame().GetRenderer().AddPointLight(ThisPtr());
}

void PointLightComponent::Unregister()
{
	Super::Unregister();
	mOwner.GetGame().GetRenderer().RemovePointLight(ThisPtr());
}

void PointLightComponent::OnUpdatedTransform()
{
	SetPosition(mOwner.GetPosition());
}

void PointLightComponent::SetProperties(const rapidjson::Value& properties)
{
	Super::SetProperties(properties);

	Vector3 diffuse;
	if (GetVectorFromJSON(properties, "diffuseColor", diffuse))
	{
		SetDiffuseColor(diffuse);
	}

	float innerRad;
	if (GetFloatFromJSON(properties, "innerRadius", innerRad))
	{
		SetInnerRadius(innerRad);
	}

	float outerRad;
	if (GetFloatFromJSON(properties, "outerRadius", outerRad))
	{
		SetOuterRadius(outerRad);
	}

	Vector3 specularCol;
	if (GetVectorFromJSON(properties, "specularColor", specularCol))
	{
		SetSpecularColor(specularCol);
	}

	float specularPow;
	if (GetFloatFromJSON(properties, "specularPower", specularPow))
	{
		SetSpecularPower(specularPow);
	}
}


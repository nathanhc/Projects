#include "ITPEnginePCH.h"

Matrix4 BoneTransform::ToMatrix() const
{
	return Matrix4::CreateFromQuaternion(mRotation) * Matrix4::CreateTranslation(mTranslation);
}

BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	BoneTransform bone;
	bone.mRotation = Slerp(a.mRotation, b.mRotation, f);
	bone.mTranslation = Lerp(a.mTranslation, b.mTranslation, f);
	return bone;
}

#include "ITPEnginePCH.h"

namespace Collision
{

	bool Intersects(const Sphere& a, const Sphere& b)
	{
		Vector3 diff = a.mCenter - b.mCenter;
		float DistSq = diff.LengthSq();
		float sumRadiiSq = (a.mRadius + b.mRadius) * (a.mRadius + b.mRadius);
		if (DistSq <= sumRadiiSq)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Intersects(const AxisAlignedBox & a, const AxisAlignedBox & b)
	{
		if (a.mMax.x < b.mMin.x || b.mMax.x < a.mMin.x) return false;
		if (a.mMax.y < b.mMin.y || b.mMax.y < a.mMin.y) return false;
		if (a.mMax.z < b.mMin.z || b.mMax.z < a.mMin.z) return false;
		return true;
	}

	bool SegmentCast(const LineSegment& segment, const AxisAlignedBox& box, Vector3& outPoint)
	{
		Vector3 ray = segment.mEnd - segment.mStart;
		float tmin = 0.0f;
		float tmax = ray.Length();

		ray.Normalize();
		
		// point p is segment.mStart
		if (Math::IsZero(std::abs(ray.x)))
		{
			// parallel, if start isn't in slab there is no collision
			if (segment.mStart.x < box.mMin.x || segment.mStart.x > box.mMax.x)
			{
				return false;
			}
		}
		else
		{
			// compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / ray.x;
			float t1 = (box.mMin.x - segment.mStart.x) * ood;
			float t2 = (box.mMax.x - segment.mStart.x) * ood;
			// make t1 be intersect with near plane, and t2 with far plane
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			// compute the intersection of slab intersection intervals
			tmin = Math::Max(tmin, t1);
			tmax = Math::Min(tmax, t2);

			// exit with no intersection as soon as slab intersection is empty
			if (tmin > tmax)
			{
				return false;
			}
		}
		if (Math::IsZero(std::abs(ray.y)))
		{
			// parallel, if start isn't in slab there is no collision
			if (segment.mStart.y < box.mMin.y || segment.mStart.y > box.mMax.y)
			{
				return false;
			}
		}
		else
		{
			// compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / ray.y;
			float t1 = (box.mMin.y - segment.mStart.y) * ood;
			float t2 = (box.mMax.y - segment.mStart.y) * ood;
			// make t1 be intersect with near plane, and t2 with far plane
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			// compute the intersection of slab intersection intervals
			tmin = Math::Max(tmin, t1);
			tmax = Math::Min(tmax, t2);

			// exit with no intersection as soon as slab intersection is empty
			if (tmin > tmax)
			{
				return false;
			}
		}
		if (Math::IsZero(std::abs(ray.z)))
		{
			// parallel, if start isn't in slab there is no collision
			if (segment.mStart.z < box.mMin.z || segment.mStart.z > box.mMax.z)
			{
				return false;
			}
		}
		else
		{
			// compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / ray.z;
			float t1 = (box.mMin.z - segment.mStart.z) * ood;
			float t2 = (box.mMax.z - segment.mStart.z) * ood;		
			// make t1 be intersect with near plane, and t2 with far plane
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			// compute the intersection of slab intersection intervals
			tmin = Math::Max(tmin, t1);
			tmax = Math::Min(tmax, t2);

			// exit with no intersection as soon as slab intersection is empty
			if (tmin > tmax)
			{
				return false;
			}
		}
		// Ray intersects all three slabs, and returns point q (outPoint) and intersection t (tmin)
		outPoint = segment.mStart + (ray * tmin);
		return true;
	}

} // namespace

#include "AABB.h"

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
	: m_min(min)
	, m_max(max) {
}

using udword = unsigned int;
#define IR(x)	((udword&)x)

Ray::HitType AABB::Hit(const Ray& ray, Ray::time_t minTime, Ray::time_t maxTime, HitRecord& record) const {
	const float epsilon = 0.00001f;
	const glm::vec3 origin = ray.Origin();
	const glm::vec3 direction = ray.Direction();

	bool inside = true;
	glm::vec3 maxT{ -1.0f };
	glm::vec3 coords{ 0.0f };

	for (uint32_t i = 0; i < 3; i++) {
		if (origin[i] < m_min[i]) {
			coords[i] = m_min[i];
			inside = false;

			if (IR(direction[i])) {
				maxT[i] = (m_min[i] - origin[i]) / direction[i];
			}
		}
		else if (origin[i] > m_max[i]) {
			coords[i] = m_max[i];
			inside = false;

			if (IR(direction[i])) {
				maxT[i] = (m_max[i] - origin[i]) / direction[i];
			}
		}
	}

	uint32_t axisIndex = static_cast<uint32_t>(Axis::x);

	if (inside) {
		coords = (m_min + m_max) / 2.0f;

		record.m_time = maxT[axisIndex];
		record.m_point = coords;
		record.m_axis = static_cast<Axis>(axisIndex);
		return Ray::HitType::Hit;
	}

	if (maxT.y > maxT[axisIndex]) axisIndex = static_cast<uint32_t>(Axis::y);
	if (maxT.z > maxT[axisIndex]) axisIndex = static_cast<uint32_t>(Axis::z);

	if (static_cast<uint32_t>(maxT[axisIndex]) & 0x80000000
		|| maxT[axisIndex] > maxTime
		|| maxT[axisIndex] < minTime) {
		return Ray::HitType::Miss;
	}

	for (uint32_t i = 0; i < 3; i++) {
		if (i != axisIndex) {
			coords[i] = origin[i] + maxT[axisIndex] * direction[i];
			if (coords[i] < m_min[i] - epsilon || coords[i] > m_max[i] + epsilon) {
				return Ray::HitType::Miss;
			}
		}
	}

	record.m_time = maxT[axisIndex];
	record.m_point = coords;
	record.m_axis = static_cast<Axis>(axisIndex);
	return Ray::HitType::Hit;
}
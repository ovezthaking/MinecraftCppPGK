#pragma once
#include "Ray.h"

#include <glm/glm.hpp>

#include <limits>

class AABB {
public:
	enum class Axis {
		x = 0,
		y = 1,
		z = 2
	};

	struct HitRecord {
		Ray::time_t m_time;
		glm::vec3 m_point;
		Axis m_axis;
	};

	AABB(const glm::vec3& min, const glm::vec3& max);

	glm::vec3 Max() const { return m_max; }
	glm::vec3 Min() const { return m_min; }

	Ray::HitType Hit(const Ray& ray, Ray::time_t minTime, Ray::time_t maxTime, HitRecord& record) const;

private:
	glm::vec3 m_min{ std::numeric_limits<float>::max() };
	glm::vec3 m_max{ std::numeric_limits<float>::min() };
};

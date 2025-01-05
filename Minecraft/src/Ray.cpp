#include "Ray.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
	: m_origin(origin)
	, m_direction(direction) {
}

glm::vec3 Ray::At(Ray::time_t t) const {
	return m_origin + m_direction * t;
}

#pragma once
#include <glm/glm.hpp>//Users/jakubstokowski/Desktop/Cube.h
#include <array>

class PerlinNoise {
public:
	PerlinNoise();
    PerlinNoise(float seed);

	float At(const glm::vec3& coords) const;

private:
	std::array<uint8_t, 256 * 2> m_permutations;
};

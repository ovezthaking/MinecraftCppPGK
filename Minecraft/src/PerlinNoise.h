#pragma once
#include <glm/glm.hpp>//Users/jakubstokowski/Desktop/Cube.h
#include <array>

class PerlinNoise {
public:
	PerlinNoise();
    PerlinNoise(int seed);

	float At(const glm::vec3& coords) const;

private:
	std::array<uint8_t, 512> m_permutations;
};

#include "PerlinNoise.h"
#include <algorithm>

namespace {
	constexpr std::array<uint8_t, 256> s_permutations = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208,89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,
		226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,
		182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,
		43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
		228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	constexpr float Fade(float t) {
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}

	constexpr float Grad(int hash, float x, float y, float z) {
		const int h = hash & 15;
		const float u = h < 8 ? x : y;
		const float v = h < 4 ? y : h == 12 || h == 14 ? x : z;

		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
}

PerlinNoise::PerlinNoise() {
	std::copy(s_permutations.begin(), s_permutations.end(), m_permutations.begin());
	std::copy(s_permutations.begin(), s_permutations.end(), m_permutations.begin() + 256);
}

PerlinNoise::PerlinNoise(float seed) {
    //...
}

float PerlinNoise::At(const glm::vec3& coords) const {
	const float _x = std::floor(coords.x);
	const float _y = std::floor(coords.y);
	const float _z = std::floor(coords.z);

	const std::int32_t ix = static_cast<std::int32_t>(_x) & 255;
	const std::int32_t iy = static_cast<std::int32_t>(_y) & 255;
	const std::int32_t iz = static_cast<std::int32_t>(_z) & 255;

	const float fx = (coords.x - _x);
	const float fy = (coords.y - _y);
	const float fz = (coords.z - _z);

	const float u = Fade(fx);
	const float v = Fade(fy);
	const float w = Fade(fz);

	const std::uint8_t A = (m_permutations[ix & 255] + iy) & 255;
	const std::uint8_t B = (m_permutations[(ix + 1) & 255] + iy) & 255;

	const std::uint8_t AA = (m_permutations[A] + iz) & 255;
	const std::uint8_t AB = (m_permutations[(A + 1) & 255] + iz) & 255;

	const std::uint8_t BA = (m_permutations[B] + iz) & 255;
	const std::uint8_t BB = (m_permutations[(B + 1) & 255] + iz) & 255;

	const float p0 = Grad(m_permutations[AA], fx, fy, fz);
	const float p1 = Grad(m_permutations[BA], fx - 1, fy, fz);
	const float p2 = Grad(m_permutations[AB], fx, fy - 1, fz);
	const float p3 = Grad(m_permutations[BB], fx - 1, fy - 1, fz);
	const float p4 = Grad(m_permutations[(AA + 1) & 255], fx, fy, fz - 1);
	const float p5 = Grad(m_permutations[(BA + 1) & 255], fx - 1, fy, fz - 1);
	const float p6 = Grad(m_permutations[(AB + 1) & 255], fx, fy - 1, fz - 1);
	const float p7 = Grad(m_permutations[(BB + 1) & 255], fx - 1, fy - 1, fz - 1);

	const float q0 = std::lerp(p0, p1, u);
	const float q1 = std::lerp(p2, p3, u);
	const float q2 = std::lerp(p4, p5, u);
	const float q3 = std::lerp(p6, p7, u);

	const float r0 = std::lerp(q0, q1, v);
	const float r1 = std::lerp(q2, q3, v);

	const float result = std::lerp(r0, r1, w);
	return (result * 0.5f) + 0.5f;
}

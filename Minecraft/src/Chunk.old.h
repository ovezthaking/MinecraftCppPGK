#pragma once
#include "Cube.h"
#include "ShaderProgram.h"
#include "PerlinNoise.h"
#include "CubePalette.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <optional>

/** Set of cubes representing fixed size chunk.
 * We associate Width dimension with X axis, Height dimension with Y axis and Depth with Z axis.
 * Order of dimensions is as follow: depth, width, heigh.
 * Each chunk has it's own 2D origin vector which represents it's world. In world coordinates
 * corresponds to first block
 */

template <uint8_t Depth, uint8_t Width, uint8_t Height>
class Chunk {
	/** Data describing single cube in a chunk.
	 * Type of cube is used when drawing together with CubePalette to obtain OpenGL objects.
	 * Cube visibility is determined based on it's 4 closest neighbours; if all of them
	 * are different than Cube::CubeType::None we consider block to be visible.
	 */

	struct CubeData {
		Cube::Type m_type{ Cube::Type::None };
		bool m_isVisible{ true };
	};

	/** Performance wise we keep all data in a flatten array.
	 * To obtain cube index based on width, heigh and depth use CoordsToIndex function
	 */

	using FlattenData_t = std::array<CubeData, Depth* Width* Height>;
public:
	/** Chunk hit record
	* Contains all data needed after ray cast to remove and place new blocks
	*/
	Chunk(const glm::vec2& origin, CubePalette& palette);

	void Generate(const PerlinNoise& rng);

	void Draw(ShaderProgram& shader) const;

private:
	size_t CoordsToIndex(size_t depth, size_t width, size_t height) const;
	void UpdateVisibility();

	CubePalette& m_palette;
	FlattenData_t m_data;
	glm::vec2 m_origin;
};

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline Chunk<Depth, Width, Height>::Chunk(const glm::vec2& origin, CubePalette& palette) {
	m_origin = origin;
	m_palette = palette;
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Generate(const PerlinNoise& rng) {
	for (size_t depth = 0; depth < Depth; ++depth) {
		for (size_t width = 0; width < Width; ++width) {
			for (size_t height = 0; height < Height; ++height) {
				const size_t index = CoordsToIndex(depth, width, height);
				m_data[index].m_type = rng.GetCubeType();
			}
		}
	}

}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Draw(ShaderProgram& shader) const {
	for (size_t depth = 0; depth < Depth; ++depth) {
		for (size_t width = 0; width < Width; ++width) {
			for (size_t height = 0; height < Height; ++height) {
				const size_t index = CoordsToIndex(depth, width, height);
				const CubeData& data = m_data[index];
				if (data.m_isVisible) {
					const glm::vec3 position = m_origin + glm::vec3(depth, width, height);
					m_palette.LookUp(data.m_type).Draw(shader, position);
				}
			}
		}
	}
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline size_t Chunk<Depth, Width, Height>::CoordsToIndex(size_t depth, size_t width, size_t height) const {
	return height * static_cast<size_t>(Depth) * static_cast<size_t>(Width)
		+ width * static_cast<size_t>(Depth)
		+ depth;
}

template<uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::UpdateVisibility() {
	for (size_t depth = 0; depth < Depth; ++depth) {
		for (size_t width = 0; width < Width; ++width) {
			for (size_t height = 0; height < Height; ++height) {
				const size_t index = CoordsToIndex(depth, width, height);
				const CubeData& data = m_data[index];
				if (data.m_type == Cube::Type::None) {
					m_data[index].m_isVisible = false;
				}
				else {
					m_data[index].m_isVisible = true;
				}
			}
		}
	}
}

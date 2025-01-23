#pragma once
#include "Cube.h"
#include "ShaderProgram.h"
#include "PerlinNoise.h"
#include "CubePalette.h"
#include "Ray.h"
#include "AABB.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
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
    struct CubeData {
        Cube::Type m_type{ Cube::Type::None };
        bool m_isVisible{ true };
    };

    using FlattenData_t = std::array<CubeData, Depth* Width* Height>;

public:
    struct HitRecord {
        glm::ivec3 m_cubeIndex;
        glm::ivec3 m_neighbourIndex;
    };

    Chunk(const glm::vec2& origin, CubePalette& palette);

    void Generate(const PerlinNoise& rng);
    void Draw(ShaderProgram& shader) const;

    Ray::HitType Hit(const Ray& ray, Ray::time_t min, Ray::time_t max,
        HitRecord& record) const;

    bool RemoveBlock(uint8_t width, uint8_t height, uint8_t depth);
    bool PlaceBlock(uint8_t width, uint8_t height, uint8_t depth, Cube::Type type);

private:
    size_t CoordsToIndex(size_t depth, size_t width, size_t height) const;
    void UpdateVisibility();
    void UpdateBlockVisibility(size_t depth, size_t width, size_t height);

    CubePalette& m_palette;
    FlattenData_t m_data;
    glm::vec2 m_origin;
    AABB m_aabb;
    std::vector<size_t> m_visibleBlocks;
};

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline Chunk<Depth, Width, Height>::Chunk(const glm::vec2& origin, CubePalette& palette)
    : m_origin(origin),
    m_palette(palette),
    m_aabb(
        glm::vec3(origin.x, 0, origin.y),
        glm::vec3(origin.x + Width, Height, origin.y + Depth))
{}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Generate(const PerlinNoise& rng) {
    float scale = 0.09f;

    for (size_t z = 0; z < Depth; ++z) {
        for (size_t x = 0; x < Width; ++x) {
            float height = rng.At(glm::vec3((m_origin.x + x) * scale, 0.0f,
                (m_origin.y + z) * scale)) *
                Height;

            for (size_t y = 0; y < Height; ++y) {
                size_t index = CoordsToIndex(z, x, y);

                if (y < height - 1) {
                    m_data[index].m_type = Cube::Type::Stone;
                }
                else if (y == static_cast<int>(height) - 1) {
                    m_data[index].m_type = Cube::Type::GrassDebug;
                }
                else {
                    m_data[index].m_type = Cube::Type::None;
                }
            }

            if (height > 0 && height < Height) {
                size_t topIndex = CoordsToIndex(z, x, static_cast<size_t>(height) - 1);
                m_data[topIndex].m_type = Cube::Type::GrassDebug;
            }
        }
    }
    UpdateVisibility();
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Draw(ShaderProgram& shader) const {
    shader.Use();

    for (size_t index : m_visibleBlocks) {
        size_t z = index % Depth;
        size_t x = (index / Depth) % Width;
        size_t y = index / (Depth * Width);

        glm::mat4 model = glm::translate(
            glm::mat4(1.0f), glm::vec3(m_origin.x + x, y, m_origin.y + z));
        shader.SetMat4("model", model);
        m_palette.LookUp(m_data[index].m_type).Draw();
    }
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline Ray::HitType Chunk<Depth, Width, Height>::Hit(const Ray& ray, Ray::time_t min, Ray::time_t max, HitRecord& record) const {
    AABB::HitRecord chunkRecord;
    if (m_aabb.Hit(ray, min, max, chunkRecord) == Ray::HitType::Miss) {
        return Ray::HitType::Miss;
    }

    glm::vec3 chunkOffset = glm::vec3(m_origin.x, 0, m_origin.y);

    Ray::time_t closestTime = max;
    bool hitDetected = false;

    for (size_t index : m_visibleBlocks) {
        size_t z = index % Depth;
        size_t x = (index / Depth) % Width;
        size_t y = index / (Depth * Width);

        glm::vec3 cubeMin = chunkOffset + glm::vec3(x, y, z);
        glm::vec3 cubeMax = cubeMin + glm::vec3(1.0f); // Cubes are 1x1x1
        AABB cubeAABB(cubeMin, cubeMax);

        AABB::HitRecord cubeRecord;
        if (cubeAABB.Hit(ray, min, closestTime, cubeRecord) == Ray::HitType::Hit) {
            closestTime = cubeRecord.m_time;
            record.m_cubeIndex = glm::ivec3(x, y, z);

            glm::ivec3 neighborOffset = glm::ivec3(0);
            if (cubeRecord.m_axis == AABB::Axis::x) {
                neighborOffset.x = (ray.Direction().x > 0) ? -1 : 1;
            }
            else if (cubeRecord.m_axis == AABB::Axis::y) {
                neighborOffset.y = (ray.Direction().y > 0) ? -1 : 1;
            }
            else if (cubeRecord.m_axis == AABB::Axis::z) {
                neighborOffset.z = (ray.Direction().z > 0) ? -1 : 1;
            }
            record.m_neighbourIndex = record.m_cubeIndex + neighborOffset;

            hitDetected = true;
        }
    }

    return hitDetected ? Ray::HitType::Hit : Ray::HitType::Miss;
}


template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline bool Chunk<Depth, Width, Height>::RemoveBlock(uint8_t width, uint8_t height, uint8_t depth) {
    size_t index = CoordsToIndex(depth, width, height);
    if (m_data[index].m_type == Cube::Type::None) {
        return false; // Blok ju¿ nie istnieje
    }
    m_data[index].m_type = Cube::Type::None; // Ustaw typ na None
    UpdateBlockVisibility(depth, width, height); // Zaktualizuj widocznoœæ s¹siadów
    return true; // Blok zosta³ usuniêty
}


template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline bool Chunk<Depth, Width, Height>::PlaceBlock(uint8_t width, uint8_t height, uint8_t depth, Cube::Type type) {
    size_t index = CoordsToIndex(depth, width, height);
    if (m_data[index].m_type != Cube::Type::None) {
        return false;
    }

    m_data[index].m_type = type;
    UpdateBlockVisibility(depth, width, height); // Zaktualizuj widocznoœæ s¹siadów
    return true;
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline size_t Chunk<Depth, Width, Height>::CoordsToIndex(size_t depth,
    size_t width,
    size_t height) const {
    return height * static_cast<size_t>(Depth) * static_cast<size_t>(Width) +
        width * static_cast<size_t>(Depth) + depth;
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::UpdateVisibility() {
    m_visibleBlocks.clear();

    for (size_t z = 0; z < Depth; ++z) {
        for (size_t x = 0; x < Width; ++x) {
            for (size_t y = 0; y < Height; ++y) {
                size_t index = CoordsToIndex(z, x, y);
                if (m_data[index].m_type == Cube::Type::None) {
                    m_data[index].m_isVisible = false;
                    continue;
                }

                bool isVisible = false;
                if (z == 0 || m_data[CoordsToIndex(z - 1, x, y)].m_type == Cube::Type::None) isVisible = true;
                if (z == Depth - 1 || m_data[CoordsToIndex(z + 1, x, y)].m_type == Cube::Type::None) isVisible = true;
                if (x == 0 || m_data[CoordsToIndex(z, x - 1, y)].m_type == Cube::Type::None) isVisible = true;
                if (x == Width - 1 || m_data[CoordsToIndex(z, x + 1, y)].m_type == Cube::Type::None) isVisible = true;
                if (y == 0 || m_data[CoordsToIndex(z, x, y - 1)].m_type == Cube::Type::None) isVisible = true;
                if (y == Height - 1 || m_data[CoordsToIndex(z, x, y + 1)].m_type == Cube::Type::None) isVisible = true;

                m_data[index].m_isVisible = isVisible;
                if (isVisible) {
                    m_visibleBlocks.push_back(index);
                }
            }
        }
    }
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::UpdateBlockVisibility(size_t depth, size_t width, size_t height) {
    auto updateNeighbor = [&](size_t d, size_t w, size_t h) {
        if (d < Depth && w < Width && h < Height) {
            UpdateVisibility(); 
        }
        };

    updateNeighbor(depth, width, height);
}

#include <unordered_map>
#include <glm/glm.hpp>
#include "Chunk.h"
#include "PerlinNoise.h"
#include "Camera.h"
#include <functional>

// Specjalizacja std::hash dla glm::ivec2
namespace std {
    template<>
    struct hash<glm::ivec2> {
        size_t operator()(const glm::ivec2& v) const noexcept {
            size_t xHash = std::hash<int>()(v.x);
            size_t yHash = std::hash<int>()(v.y);
            return xHash ^ (yHash << 1); // Prosta kombinacja hashów
        }
    };
}




const int CHUNK_SIZE = 16;
const int RENDER_DISTANCE = 3; // Number of chunks to render around the player

class ChunkManager {
public:
    ChunkManager(CubePalette& palette, const PerlinNoise& perlin)
        : m_palette(palette), m_perlin(perlin) {}

    void Update(const glm::vec3& playerPosition) {
        glm::ivec2 playerChunk = WorldToChunkCoords(playerPosition);

        // Load new chunks
        for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; ++x) {
            for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; ++z) {
                glm::ivec2 chunkCoords = playerChunk + glm::ivec2(x, z);
                if (m_chunks.find(chunkCoords) == m_chunks.end()) {
                    m_chunks[chunkCoords] = std::make_unique<Chunk<CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE>>(glm::vec2(chunkCoords.x * CHUNK_SIZE, chunkCoords.y * CHUNK_SIZE), m_palette);
                    m_chunks[chunkCoords]->Generate(m_perlin);
                }
            }
        }

        // Unload distant chunks
        for (auto it = m_chunks.begin(); it != m_chunks.end();) {
            glm::ivec2 chunkCoords = it->first;
            if (glm::abs(chunkCoords.x - playerChunk.x) > RENDER_DISTANCE || glm::abs(chunkCoords.y - playerChunk.y) > RENDER_DISTANCE) {
                it = m_chunks.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void Draw(ShaderProgram& shader) const {
        for (const auto& pair : m_chunks) {
            pair.second->Draw(shader);
        }
    }

private:
    glm::ivec2 WorldToChunkCoords(const glm::vec3& position) const {
        return glm::ivec2(floor(position.x / CHUNK_SIZE), floor(position.z / CHUNK_SIZE));
    }

    CubePalette& m_palette;
    const PerlinNoise& m_perlin;
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk<CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE>>> m_chunks;
};

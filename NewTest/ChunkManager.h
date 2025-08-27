#pragma once
#include "Chunk.h"
#include <map>


/*
Used to create, render, place/remove blocks from chunks.
*/
struct ChunkManager {
public:
    ChunkManager(const float* verticies, size_t count, const glm::vec3* cameraPosition, int renderDistance, unsigned int textureAtlasID) {
        m_verts = verticies;
        m_count = count;
        m_renderDistance = renderDistance;
        m_cameraPosition = cameraPosition;
        m_textureAtlasID = textureAtlasID;
    }

    void DrawChunks() {
        std::vector<std::reference_wrapper<Chunk>> chunks = GetChunks();
        for (int i = 0; i < chunks.size(); i++) {
            Chunk& chunk = chunks[i];
            chunk.Draw();
        }
    }

    std::vector<std::reference_wrapper<Chunk>> GetChunks() {
        std::vector<std::reference_wrapper<Chunk>> visible;

        glm::ivec3 playerChunck = GetChunckCoords(16);

        for (int dx = -m_renderDistance; dx <= m_renderDistance; dx++) {
            for (int dz = -m_renderDistance; dz <= m_renderDistance; dz++) {
                int chunkX = playerChunck.x + dx;
                int chunkZ = playerChunck.z + dz;

                glm::vec3 chunkWorldPos = glm::vec3(chunkX * 16, -200, chunkZ * 16);

                auto& innerMap = m_chunks2.try_emplace(chunkX).first->second;

                auto [it, inserted] = innerMap.try_emplace(chunkZ, m_verts, m_count, chunkWorldPos);

                if (inserted) { //A new chunk was created and needs to be initialized
                    Chunk& newChunk = it->second;
                    newChunk.InitGl(m_textureAtlasID);
                    std::array<Chunk*, 4> surroundingChunks = SurroundingChunks2(chunkX, chunkZ);
                    for (Chunk* chunk : surroundingChunks) {
                        if (chunk == nullptr) continue;
                        glm::vec3 origin = chunk->GetOrigin();
                        int chunkXNew = origin.x / 16;
                        int chunkZNew = origin.z / 16;
                        std::array<Chunk*, 4> newSurroundingChunks = SurroundingChunks2(chunkXNew, chunkZNew);
                        chunk->SetSurroundingChunks(newSurroundingChunks);
                    }
                    it->second.SetSurroundingChunks(surroundingChunks);
                }

                visible.push_back(it->second);
            }
        }
        return visible;
    }

    //Given a position, place the block within chunk
    bool PlaceBlock(glm::vec3 placePosition, Blocks blockType, int posX, int posZ) {
        int chunkX = floor(posX / 16);
        int chunkZ = floor(posZ / 16);

        int placeX = posX - (16 * chunkX);
        int placeZ = posZ - (16 * chunkZ);
        m_chunks2.at(chunkX).at(chunkZ).PlaceBlock(glm::vec3(placeX, 16, placeZ), blockType);
        ReloadSurrondingChunks(chunkX, chunkZ);
        return true;
    }

    //Given a position, remove a block within chunk
    bool RemoveBlock(glm::vec3 removePosition, int posX, int posZ) {
        int chunkX = floor(posX / 16);
        int chunkZ = floor(posZ / 16);
        m_chunks2.at(chunkX).at(chunkZ).RemoveBlock(removePosition);
        ReloadSurrondingChunks(chunkX, chunkZ);
        return true;
    }

private:
    //Recalculate the visible verts in the chunks surrounding a given chunk
    void ReloadSurrondingChunks(int dx, int dz) {
        std::array<Chunk*, 4> surroundingChunks = SurroundingChunks2(dx, dz);
        for (const auto& Chunk : surroundingChunks) {
            Chunk->GetVisibleVerts();
        }
    }

    //Convert coordinates from world coords to matrix coords
    glm::ivec3 GetChunckCoords(int chunkSize) {
        return glm::ivec3(
            std::floor(m_cameraPosition->x / chunkSize),
            std::floor(0),
            std::floor(m_cameraPosition->z / chunkSize)
        );
    }

    //Determine which chunks are created surrounding the chunk at x/y
    std::array<Chunk*, 4> SurroundingChunks2(int x, int z) {
        return { IsChunkHere(x + 1, z), IsChunkHere(x - 1, z), IsChunkHere(x, z + 1), IsChunkHere(x, z - 1) };
    }

    Chunk* IsChunkHere(int x, int z) {
        auto itX = m_chunks2.find(x);
        if (itX != m_chunks2.end()) {
            auto itY = itX->second.find(z);
            if (itY != itX->second.end()) {
                return &itY->second;
            }
        }
        return nullptr;
    }

    const float* m_verts;
    int m_renderDistance;
    size_t m_count;
    const glm::vec3* m_cameraPosition;

    std::map<int, std::map<int, Chunk>> m_chunks2{};

    unsigned int m_textureAtlasID;
};
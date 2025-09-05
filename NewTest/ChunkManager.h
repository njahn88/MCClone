#pragma once
#include "Chunk.h"
#include <map>
#include <unordered_map>


struct ChunkCoords {
    ChunkCoords(int x, int z) {
        this->x = x;
        this->z = z;
    }
    int x, z;

    bool operator==(const ChunkCoords& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkCoordsHash {
    std::size_t operator()(const ChunkCoords& coord) const {
        return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.z) << 1);
    }
};

/*
Used to create, render, place/remove blocks from chunks.
*/
struct ChunkManager {
public:
    ChunkManager(const float* verts, unsigned int textureAtlasID, int renderDistance, glm::vec3* cameraPosition) : m_verts(verts), m_textureAtlasID(textureAtlasID), m_renderDistance(renderDistance), m_cameraPosition(cameraPosition) {};

    void Update() {
        OperateOnSurroundingChunks([this](int x, int z) {InitChunk(x, z); });
        OperateOnSurroundingChunks([this](int x, int z) {UpdateVisibleVerts(x, z); });
        OperateOnSurroundingChunks([this](int x, int z) {RenderChunk(x, z); });
    }
private:
    inline ChunkCoords WorldToChunkCoords(const glm::vec3* worldPos) {
        int chunkX = static_cast<int>(std::floor(worldPos->x / 16));
        int chunkZ = static_cast<int>(std::floor(worldPos->z / 16));
        return { chunkX, chunkZ };
    }

    template<typename Func>
    void OperateOnSurroundingChunks(Func operation) {
        ChunkCoords currentChunkCoords = WorldToChunkCoords(m_cameraPosition);
        for (int x = currentChunkCoords.x - m_renderDistance; x < currentChunkCoords.x + m_renderDistance; x++) {
            for (int z = currentChunkCoords.z - m_renderDistance; z < currentChunkCoords.z + m_renderDistance; z++) {
                operation(x, z);
            }
        }
    }

    void InitChunk(int x, int z) {
        ChunkCoords currentChunkCoords{ x, z };
        CreateChunk(currentChunkCoords);
    }

    void CreateChunk(ChunkCoords position) {
        auto it = chunkMap.find(position);
        if (it == chunkMap.end()) { //Chunk doesnt exist
            chunkMap[position] = new Chunk{ m_verts, ChunkWorldCoordinates{position.x * 16, position.z * 16},  m_textureAtlasID}; //Create new chunk at position
            chunkMap[position]->InitGL();
            chunkMap[position]->GenerateAsync();
            std::cout << "Creating new chunk" << std::endl;
        }
    }

    void UpdateVisibleVerts(int x, int z) {
        ChunkCoords currentChunkCoords{ x, z };
        if (chunkMap[currentChunkCoords]->HasFinishedGeneratingBlocks()) {
            chunkMap[currentChunkCoords]->GenerateVisibleVertsAsync();
        }
    }

    void RenderChunk(int x, int z) {
        ChunkCoords currentChunkCoords{ x, z };
        if (chunkMap[currentChunkCoords]->HasFinishedGeneratingVisibleVerts()) {
            chunkMap[currentChunkCoords]->RenderChunk();
        }
    }
    const float* m_verts;
    int m_renderDistance;
    size_t m_count;
    const glm::vec3* m_cameraPosition;

    std::map<int, std::map<int, Chunk>> m_chunks2{};

    std::unordered_map<ChunkCoords, Chunk*, ChunkCoordsHash> chunkMap;

    unsigned int m_textureAtlasID;
};
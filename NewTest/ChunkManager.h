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

struct Plane {
    float a, b, c, d;
    void normalize() {
        float len = sqrt(a * a + b * b + c * c);
        a /= len; b /= len; c /= len; d /= len;
    }
};

/*
Used to create, render, place/remove blocks from chunks.
*/
struct ChunkManager {
public:
    ChunkManager(const float* verts, unsigned int textureAtlasID, int renderDistance, glm::vec3* cameraPosition) : m_verts(verts), m_textureAtlasID(textureAtlasID), m_renderDistance(renderDistance), m_cameraPosition(cameraPosition) {};

    void Update(glm::mat4 VP) {
        UpdateFrustum(VP);
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
        }
    }

    void UpdateVisibleVerts(int x, int z) {
        ChunkCoords currentChunkCoords{ x, z };
        if (chunkMap[currentChunkCoords]->HasFinishedGeneratingBlocks()) {
            std::array<Chunk*, 4> surroundingChunks = GetSurroundingChunks(currentChunkCoords);
            chunkMap[currentChunkCoords]->GenerateVisibleVertsAsync(surroundingChunks);
        }
    }

    void RenderChunk(int x, int z) {
        ChunkCoords currentChunkCoords{ x, z };
        if (chunkMap[currentChunkCoords]->HasFinishedGeneratingVisibleVerts()) {
            Chunk* currentChunk = chunkMap[currentChunkCoords];
            if (isChunkInFrustum(currentChunk->GetMin(), currentChunk->GetMax())) {
                chunkMap[currentChunkCoords]->RenderChunk();
            }
        }
    }
    std::array<Chunk*, 4> GetSurroundingChunks(ChunkCoords currentChunkCoords) {
        std::array<Chunk*, 4> surroundingChunks{ nullptr, nullptr, nullptr, nullptr };
        ChunkCoords rightChunkCoords{ currentChunkCoords.x + 1, currentChunkCoords.z };
        auto it = chunkMap.find(rightChunkCoords);
        if (it != chunkMap.end()) { //Right chunk exists
            surroundingChunks[0] = chunkMap[rightChunkCoords];
        }
        ChunkCoords leftChunkCoords{ currentChunkCoords.x - 1, currentChunkCoords.z };
        auto it2 = chunkMap.find(leftChunkCoords);
        if (it2 != chunkMap.end()) {
            surroundingChunks[1] = chunkMap[leftChunkCoords];
        }
        ChunkCoords frontChunkCoords{ currentChunkCoords.x, currentChunkCoords.z + 1 };
        auto it3 = chunkMap.find(frontChunkCoords);
        if (it3 != chunkMap.end()) {
            surroundingChunks[2] = chunkMap[frontChunkCoords];
        }
        ChunkCoords backChunkCoords{ currentChunkCoords.x, currentChunkCoords.z - 1 };
        auto it4 = chunkMap.find(backChunkCoords);
        if (it4 != chunkMap.end()) {
            surroundingChunks[3] = chunkMap[backChunkCoords];
        }
        return surroundingChunks;
    }

    void UpdateFrustum(glm::mat4 VP) {
        frustum[0] = { VP[0][3] + VP[0][0], VP[1][3] + VP[1][0], VP[2][3] + VP[2][0], VP[3][3] + VP[3][0] };
        // Right
        frustum[1] = { VP[0][3] - VP[0][0], VP[1][3] - VP[1][0], VP[2][3] - VP[2][0], VP[3][3] - VP[3][0] };
        // Bottom
        frustum[2] = { VP[0][3] + VP[0][1], VP[1][3] + VP[1][1], VP[2][3] + VP[2][1], VP[3][3] + VP[3][1] };
        // Top
        frustum[3] = { VP[0][3] - VP[0][1], VP[1][3] - VP[1][1], VP[2][3] - VP[2][1], VP[3][3] - VP[3][1] };
        // Near
        frustum[4] = { VP[0][3] + VP[0][2], VP[1][3] + VP[1][2], VP[2][3] + VP[2][2], VP[3][3] + VP[3][2] };
        // Far
        frustum[5] = { VP[0][3] - VP[0][2], VP[1][3] - VP[1][2], VP[2][3] - VP[2][2], VP[3][3] - VP[3][2] };

        for (int i = 0; i < 6; i++) frustum[i].normalize();
    }

    bool isChunkInFrustum(const glm::vec3& min, const glm::vec3& max) {
        for (int i = 0; i < 6; i++) {
            const Plane& p = frustum[i];

            glm::vec3 positive = {
                p.a > 0 ? max.x : min.x,
                p.b > 0 ? max.y : min.y,
                p.c > 0 ? max.z : min.z
            };
            if (p.a * positive.x + p.b * positive.y + p.c * positive.z + p.d < 0) {
                return false;
            }
        }
        return true;
    }

    const float* m_verts;
    int m_renderDistance;
    size_t m_count;
    const glm::vec3* m_cameraPosition;

    std::map<int, std::map<int, Chunk>> m_chunks2{};

    std::unordered_map<ChunkCoords, Chunk*, ChunkCoordsHash> chunkMap;

    unsigned int m_textureAtlasID;
    Plane frustum[6];
};
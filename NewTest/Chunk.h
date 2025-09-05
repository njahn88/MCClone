#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Block.h"
#include <cstdlib>
#include "Perlin.h"
#include <thread>

//Structure for which faces on the block should be visible
struct VisibleFaces {
    bool front = true;
    bool back = true;
    bool left = true;
    bool right = true;
    bool top = true;
    bool bottom = true;
};


struct ChunkWorldCoordinates {
public:
    ChunkWorldCoordinates(int xWorldPosition, int zWorldPosition) : xWorldPosition(xWorldPosition), zWorldPosition(zWorldPosition) {};
    int xWorldPosition, zWorldPosition;
};

struct Chunk {
public:
    Chunk(const float* verts, ChunkWorldCoordinates chunkCoordinates, unsigned int textureAtlasID) : m_verts(verts), m_chunkWorldCoords(chunkCoordinates), m_textureAtlasID(textureAtlasID){}

    void InitGL() {
        glGenVertexArrays(1, &m_Vao);
        glBindVertexArray(m_Vao);

        glGenBuffers(1, &m_Vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);


        glBindTexture(GL_TEXTURE_2D, m_textureAtlasID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void GenerateAsync() {
        std::thread t(&Chunk::InitChunkData, this);
        t.detach();
    }

    void GenerateVisibleVertsAsync(std::array<Chunk*, 4> surroundingChunks) {
        if (!m_generating_visible_verts) {
            m_surroundingChunks = surroundingChunks;
            std::cout << "Generating visible verts" << std::endl;
            std::thread t(&Chunk::GenerateVisibleVerts, this);
            t.detach();
            m_generating_visible_verts = true;
        }
    }

    bool HasFinishedGeneratingBlocks() const {
        return m_finished_generating_blocks;
    }

    bool HasFinishedGeneratingVisibleVerts() const {
        return m_finished_generating_visible_verts;
    }
    
    void RenderChunk() {
        glBindVertexArray(m_Vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

        if (!m_have_chached_visible_verts) {
            glBufferData(GL_ARRAY_BUFFER, m_visibleVerts.size() * sizeof(float), m_visibleVerts.data(), GL_STATIC_DRAW);
            m_have_chached_visible_verts = true;
        }
        glDrawArrays(GL_TRIANGLES, 0, m_visibleVerts.size() / 5);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    bool IsBlockPresent(int x, int y, int z) {
        return !m_blocks[x][y][z].IsAirBlock();
    }
private:
    void InitChunkData() {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                double n = Perlin::perlinOctave((m_chunkWorldCoords.xWorldPosition + x) * 0.01,
                    (m_chunkWorldCoords.zWorldPosition + z) * 0.01,
                    4,
                    0.5,
                    2.0
                );

                for (int y = 0; y <= n; y++) {
                    m_blocks[x][y][z] = blocks[Blocks::GRASS_BLOCK];
                }

                for (int h = n + 1; h <= 199; h++) {
                    m_blocks[x][h][z] = blocks[Blocks::AIR_BLOCK];
                }

            }
        }
        m_finished_generating_blocks = true;
    }

    void GenerateVisibleVerts() {
        m_visibleVerts.clear();
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 200; y++) {
                for (int z = 0; z < 16; z++) {
                    VisibleFaces visibleFaces = GetVisibleFaces(x, y, z);
                    Block currentBlock = m_blocks[x][y][z];

                    float atlasFactor = 1.0f / 16.0f;


                    if (visibleFaces.back) {
                        int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                        for (int i = 0; i < 30; i += 5) {
                            double xTextCoord = (m_verts[i + 3] * atlasFactor) + currentBlock.GetSideTextureIndex() * atlasFactor;
                            double yTextCoord = (m_verts[i + 4] * atlasFactor) + atlasYIndex * atlasFactor;
                            m_visibleVerts.push_back((m_verts[i] + x) + m_chunkWorldCoords.xWorldPosition);
                            m_visibleVerts.push_back((m_verts[i + 1] + y));
                            m_visibleVerts.push_back((m_verts[i + 2] + z) + m_chunkWorldCoords.zWorldPosition);
                            m_visibleVerts.push_back(xTextCoord);
                            m_visibleVerts.push_back(yTextCoord);
                        }
                    }
                    if (visibleFaces.front) {
                        int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                        for (int i = 30; i < 60; i += 5) {
                            double xTextCoord = (m_verts[i + 3] * atlasFactor) + currentBlock.GetSideTextureIndex() * atlasFactor;
                            double yTextCoord = (m_verts[i + 4] * atlasFactor) + atlasYIndex * atlasFactor;
                            m_visibleVerts.push_back((m_verts[i] + x) + m_chunkWorldCoords.xWorldPosition);
                            m_visibleVerts.push_back((m_verts[i + 1] + y));
                            m_visibleVerts.push_back((m_verts[i + 2] + z) + m_chunkWorldCoords.zWorldPosition);
                            m_visibleVerts.push_back(xTextCoord);
                            m_visibleVerts.push_back(yTextCoord);
                        }
                    }
                    if (visibleFaces.left) {
                        int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                        for (int i = 60; i < 90; i += 5) {
                            double xTextCoord = (m_verts[i + 3] * atlasFactor) + currentBlock.GetSideTextureIndex() * atlasFactor;
                            double yTextCoord = (m_verts[i + 4] * atlasFactor) + atlasYIndex * atlasFactor;
                            m_visibleVerts.push_back((m_verts[i] + x) + m_chunkWorldCoords.xWorldPosition);
                            m_visibleVerts.push_back((m_verts[i + 1] + y));
                            m_visibleVerts.push_back((m_verts[i + 2] + z) + m_chunkWorldCoords.zWorldPosition);
                            m_visibleVerts.push_back(xTextCoord);
                            m_visibleVerts.push_back(yTextCoord);
                        }
                    }
                    if (visibleFaces.right) {
                        int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                        for (int i = 90; i < 120; i += 5) {
                            double xTextCoord = (m_verts[i + 3] * atlasFactor) + currentBlock.GetSideTextureIndex() * atlasFactor;
                            double yTextCoord = (m_verts[i + 4] * atlasFactor) + atlasYIndex * atlasFactor;
                            m_visibleVerts.push_back((m_verts[i] + x) + m_chunkWorldCoords.xWorldPosition);
                            m_visibleVerts.push_back((m_verts[i + 1] + y));
                            m_visibleVerts.push_back((m_verts[i + 2] + z) + m_chunkWorldCoords.zWorldPosition);
                            m_visibleVerts.push_back(xTextCoord);
                            m_visibleVerts.push_back(yTextCoord);
                        }
                    }
                    if (visibleFaces.bottom) {
                        int atlasYIndex = currentBlock.GetBottomTextureIndex() / 16;
                        for (int i = 120; i < 150; i += 5) {
                            double xTextCoord = (m_verts[i + 3] * atlasFactor) + currentBlock.GetBottomTextureIndex() * atlasFactor;
                            double yTextCoord = (m_verts[i + 4] * atlasFactor) + atlasYIndex * atlasFactor;
                            m_visibleVerts.push_back((m_verts[i] + x) + m_chunkWorldCoords.xWorldPosition);
                            m_visibleVerts.push_back((m_verts[i + 1] + y));
                            m_visibleVerts.push_back((m_verts[i + 2] + z) + m_chunkWorldCoords.zWorldPosition);
                            m_visibleVerts.push_back(xTextCoord);
                            m_visibleVerts.push_back(yTextCoord);
                        }
                    }
                    if (visibleFaces.top) {
                        int atlasYIndex = currentBlock.GetTopTextureIndex() / 16;
                        for (int i = 150; i < 180; i += 5) {
                            double xTextCoord = (m_verts[i + 3] * atlasFactor) + currentBlock.GetTopTextureIndex() * atlasFactor;
                            double yTextCoord = (m_verts[i + 4] * atlasFactor) + atlasYIndex * atlasFactor;
                            m_visibleVerts.push_back((m_verts[i] + x) + m_chunkWorldCoords.xWorldPosition);
                            m_visibleVerts.push_back((m_verts[i + 1] + y));
                            m_visibleVerts.push_back((m_verts[i + 2] + z) + m_chunkWorldCoords.zWorldPosition);
                            m_visibleVerts.push_back(xTextCoord);
                            m_visibleVerts.push_back(yTextCoord);
                        }
                    }
                }
            }
        }
        m_finished_generating_visible_verts = true;
        m_have_chached_visible_verts = false;
    }

    VisibleFaces GetVisibleFaces(int x, int y, int z) {
        VisibleFaces visibleFaces;

        if (m_blocks[x][y][z].IsAirBlock()) return { false, false, false, false, false, false };

        if (x > 0) {
            if (!m_blocks[x - 1][y][z].IsAirBlock()) {
                visibleFaces.left = false;
            }
        }
        else {
            if (m_surroundingChunks[1]) {
                if (m_surroundingChunks[1]->HasFinishedGeneratingBlocks()) {
                    if (m_surroundingChunks[1]->IsBlockPresent(15, y, z)) {
                        visibleFaces.left = false;
                    }
                }
            }
            else {
                visibleFaces.left = true;
            }
        }

        if (x < 15) {
            if (!m_blocks[x + 1][y][z].IsAirBlock()) {
                visibleFaces.right = false;
            }
        }
        else {
            if (m_surroundingChunks[0]) {
                if (m_surroundingChunks[0]->HasFinishedGeneratingBlocks()) {
                    if (m_surroundingChunks[0]->IsBlockPresent(0, y, z)) {
                        visibleFaces.right = false;
                    }
                }
            }
            else {
                visibleFaces.right = true;
            }
        }

        if (y > 0 && !m_blocks[x][y - 1][z].IsAirBlock()) {
            visibleFaces.bottom = false;
        }
        if (y < 199 && !m_blocks[x][y + 1][z].IsAirBlock()) {
            visibleFaces.top = false;
        }

        if (z > 0) {
            if (!m_blocks[x][y][z - 1].IsAirBlock()) {
                visibleFaces.back = false;
            }
        }
        else {
            if (m_surroundingChunks[3]) {
                if (m_surroundingChunks[3]->HasFinishedGeneratingBlocks()) {
                    if (m_surroundingChunks[3]->IsBlockPresent(x, y, 15)) {
                        visibleFaces.back = false;
                    }
                }
            }
            else {
                visibleFaces.back = true;
            }
        }

        if (z < 15) {
            if (!m_blocks[x][y][z + 1].IsAirBlock()) {
                visibleFaces.front = false;
            }
        }
        else {
            if (m_surroundingChunks[2]) {
                if (m_surroundingChunks[2]->HasFinishedGeneratingBlocks()) {
                    if (m_surroundingChunks[2]->IsBlockPresent(x, y, 0)) {
                        visibleFaces.front = false;
                    }
                }
            }
            else {
                visibleFaces.front = true;
            }
        }
        return visibleFaces;
    }

    Block m_blocks[16][200][16];
    bool m_finished_generating_blocks = false;
    bool m_finished_generating_visible_verts = false;
    unsigned int m_Vbo, m_Vao;
    std::vector<float> m_visibleVerts{};
    const float* m_verts;
    ChunkWorldCoordinates m_chunkWorldCoords;
    bool m_generating_visible_verts = false;
    unsigned int m_textureAtlasID;
    bool m_have_chached_visible_verts = false;

    std::array<Chunk*, 4> m_surroundingChunks{nullptr, nullptr, nullptr, nullptr};
};
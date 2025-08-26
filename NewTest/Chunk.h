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

//Structure for which faces on the block should be visible
struct VisibleFaces {
    bool front, back, left, right, top, bottom;
};


struct Chunk {
public:
    Chunk(const double* verticies, size_t count, glm::vec3 chunkOrigin) {
        int xOrigin = chunkOrigin.x;
        int yOrigin = chunkOrigin.y;
        int zOrigin = chunkOrigin.z;
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                /*
                double n = Perlin::perlin((chunkOrigin.x + x) * 0.01,
                    (chunkOrigin.z + z) * 0.01);
                */
                double n = Perlin::perlinOctave((chunkOrigin.x + x) * 0.01,
                    (chunkOrigin.z + z) * 0.01,
                    4,
                    0.5,
                    2.0
                );

                for (int y = 0; y <= n; y++) {
                    SetBlock(x, y, z, Blocks::GRASS_BLOCK);
                }
                
                for (int h = n + 1; h <= 255; h++) {
                    SetBlock(x, h, z, Blocks::AIR_BLOCK);
                }
                
            }
        }
        m_chunkOrigin = chunkOrigin;
        m_verticies.assign(verticies, verticies + count);

    }

    void InitGl(unsigned int textureAtlasID) {

        glGenVertexArrays(1, &m_Vao);
        glBindVertexArray(m_Vao);


        glGenBuffers(1, &m_Vbo);
        GetVisibleVerts();


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindTexture(GL_TEXTURE_2D, textureAtlasID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

        glBindVertexArray(0);

    }

    //Determine which verticies are visible based on surrounding blocks and chunks
    void GetVisibleVerts() {
        std::vector<float> visibleVertices{};
        for (unsigned int i = 0; i < m_cubePositions.size(); i++) {
            int x = static_cast<int>(m_cubePositions[i].x);
            int y = static_cast<int>(m_cubePositions[i].y);
            int z = static_cast<int>(m_cubePositions[i].z);

            VisibleFaces faces = GetVisibleFaces(x, y, z);

            unsigned int blockIndex = blockType[x][y][z];
            Block currentBlock = blocks[blockIndex];

            float blockTextureWidth = 1.0f / 16.0f;

            if (faces.back) { // Back face (-z), offset 0–29
                for (int v = 0; v < 30; v += 5) {
                    int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                    double xTextCoord = (m_verticies[v + 3] * blockTextureWidth) + currentBlock.GetSideTextureIndex() * blockTextureWidth;
                    double yTextCoord = (m_verticies[v + 4] * blockTextureWidth) + atlasYIndex * blockTextureWidth;
                    visibleVertices.push_back((m_verticies[v] + x) + m_chunkOrigin.x);
                    visibleVertices.push_back((m_verticies[v + 1] + y) + m_chunkOrigin.y);
                    visibleVertices.push_back((m_verticies[v + 2] + z) + m_chunkOrigin.z);
                    visibleVertices.push_back(xTextCoord);
                    visibleVertices.push_back(yTextCoord);
                }
            }
            if (faces.front) { // Front face (+z), offset 30–59
                for (int v = 30; v < 60; v += 5) {
                    int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                    double xTextCoord = (m_verticies[v + 3] * blockTextureWidth) + currentBlock.GetSideTextureIndex() * blockTextureWidth;
                    double yTextCoord = (m_verticies[v + 4] * blockTextureWidth) + atlasYIndex * blockTextureWidth;
                    visibleVertices.push_back((m_verticies[v] + x) + m_chunkOrigin.x);
                    visibleVertices.push_back((m_verticies[v + 1] + y) + m_chunkOrigin.y);
                    visibleVertices.push_back((m_verticies[v + 2] + z) + m_chunkOrigin.z);
                    visibleVertices.push_back(xTextCoord);
                    visibleVertices.push_back(yTextCoord);
                }
            }
            if (faces.left) { // Left face (-x), offset 60–89
                for (int v = 60; v < 90; v += 5) {
                    int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                    double xTextCoord = (m_verticies[v + 3] * blockTextureWidth) + currentBlock.GetSideTextureIndex() * blockTextureWidth;
                    double yTextCoord = (m_verticies[v + 4] * blockTextureWidth) + atlasYIndex * blockTextureWidth;
                    visibleVertices.push_back((m_verticies[v] + x) + m_chunkOrigin.x);
                    visibleVertices.push_back((m_verticies[v + 1] + y) + m_chunkOrigin.y);
                    visibleVertices.push_back((m_verticies[v + 2] + z) + m_chunkOrigin.z);
                    visibleVertices.push_back(xTextCoord);
                    visibleVertices.push_back(yTextCoord);
                }
            }
            if (faces.right) { // Right face (+x), offset 90–119
                for (int v = 90; v < 120; v += 5) {
                    int atlasYIndex = currentBlock.GetSideTextureIndex() / 16;
                    double xTextCoord = (m_verticies[v + 3] * blockTextureWidth) + currentBlock.GetSideTextureIndex() * blockTextureWidth;
                    double yTextCoord = (m_verticies[v + 4] * blockTextureWidth) + atlasYIndex * blockTextureWidth;
                    visibleVertices.push_back((m_verticies[v] + x) + m_chunkOrigin.x);
                    visibleVertices.push_back((m_verticies[v + 1] + y) + m_chunkOrigin.y);
                    visibleVertices.push_back((m_verticies[v + 2] + z) + +m_chunkOrigin.z);
                    visibleVertices.push_back(xTextCoord);
                    visibleVertices.push_back(yTextCoord);
                }
            }
            if (faces.bottom) { // Bottom face (-y), offset 120–149
                for (int v = 120; v < 150; v += 5) {
                    int atlasYIndex = currentBlock.GetBottomTextureIndex() / 16;
                    double xTextCoord = (m_verticies[v + 3] * blockTextureWidth) + currentBlock.GetBottomTextureIndex() * blockTextureWidth;
                    double yTextCoord = (m_verticies[v + 4] * blockTextureWidth) + atlasYIndex * blockTextureWidth;
                    visibleVertices.push_back((m_verticies[v] + x) + m_chunkOrigin.x);
                    visibleVertices.push_back((m_verticies[v + 1] + y) + m_chunkOrigin.y);
                    visibleVertices.push_back((m_verticies[v + 2] + z) + +m_chunkOrigin.z);
                    visibleVertices.push_back(xTextCoord);
                    visibleVertices.push_back(yTextCoord);
                }
            }
            if (faces.top) { // Top face (+y), offset 150–179
                for (int v = 150; v < 180; v += 5) {
                    int atlasYIndex = currentBlock.GetTopTextureIndex() / 16;
                    double xTextCoord = (m_verticies[v + 3] * blockTextureWidth) + currentBlock.GetTopTextureIndex() * blockTextureWidth;
                    double yTextCoord = (m_verticies[v + 4] * blockTextureWidth) + atlasYIndex * blockTextureWidth;
                    visibleVertices.push_back((m_verticies[v] + x) + m_chunkOrigin.x);
                    visibleVertices.push_back((m_verticies[v + 1] + y) + m_chunkOrigin.y);
                    visibleVertices.push_back((m_verticies[v + 2] + z) + m_chunkOrigin.z);
                    visibleVertices.push_back(xTextCoord);
                    visibleVertices.push_back(yTextCoord);
                }
            }
        }
        m_shouldCalcVisibleVertsFlag = false;
        m_cachedVisibleVerts = visibleVertices;
        glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
        glBufferData(GL_ARRAY_BUFFER, m_cachedVisibleVerts.size() * sizeof(float), m_cachedVisibleVerts.data(), GL_STATIC_DRAW);
    }

    //Finds the block at the position in the chunk and removes it if it exists
    bool RemoveBlock(glm::vec3 removePosition) {
        auto it = std::find(m_cubePositions.begin(), m_cubePositions.end(), removePosition);

        int xPosition = removePosition.x;
        int yPosition = removePosition.y;
        int zPosition = removePosition.z;

        if (it != m_cubePositions.end()) {
            m_cubePositions.erase(it);
            cubeGrid[xPosition][yPosition][zPosition] = false;
            GetVisibleVerts();
            return true;
        }
        return false;
    }

    //Places / updates block at given position within chunk
    bool PlaceBlock(glm::vec3 placeLocation, Blocks newBlockType) {
        auto it = std::find(m_cubePositions.begin(), m_cubePositions.end(), placeLocation);

        int xLocation = placeLocation.x;
        int yLocation = placeLocation.y;
        int zLocation = placeLocation.z;

        if (it != m_cubePositions.end()) { //There is currently a block there, replace it
            blockType[xLocation][yLocation][zLocation] = newBlockType;
        }
        else { //empty space
            m_cubePositions.push_back(placeLocation);
            blockType[xLocation][yLocation][zLocation] = newBlockType;
            cubeGrid[xLocation][yLocation][zLocation] = true;
        }
        GetVisibleVerts();
        return true;
    }

    glm::vec3 GetOrigin() {
        return m_chunkOrigin;
    }

    void SetSurroundingChunks(std::array<Chunk*, 4> surroundingChunks) {
        m_surroundingChunks = surroundingChunks;
        GetVisibleVerts();
    }

    void UpdateBlock(int x, int y, int z) {

    }

    bool BlockPresent(int x, int y, int z) {
        return blockType[x][y][z] != Blocks::AIR_BLOCK;
    }

    bool(&GetCubeGrid())[16][256][16]{
        return cubeGrid;
    }

        Blocks GetBlockType(int x, int y, int z) {
        return static_cast<Blocks>(blockType[x][y][z]);
    }

    void RecalculateVisibleVerts() {
        m_shouldCalcVisibleVertsFlag = true;
    }

    void Draw() {
        glBindVertexArray(m_Vao);
        glDrawArrays(GL_TRIANGLES, 0, m_cachedVisibleVerts.size());
        glBindVertexArray(0);
    }

private:

    //Return the visible faces based on surrounding blocks and chunks
    VisibleFaces GetVisibleFaces(int x, int y, int z) {
        VisibleFaces faces = { false, false, false, false, false, false };

        if (blockType[x][y][z] == Blocks::AIR_BLOCK) return faces;


        if (x == 0) {
            if (m_surroundingChunks[1] != nullptr) {
                if (m_surroundingChunks[1]->BlockPresent(15, y, z)) {
                    Blocks adjacentBlock = m_surroundingChunks[1]->GetBlockType(15, y, z);
                    if (adjacentBlock == Blocks::AIR_BLOCK && blockType[x][y][z] != Blocks::AIR_BLOCK) {
                        faces.left = true;
                    }
                    else {
                        faces.left = false;
                    }
                }
                else {
                    faces.left = true;
                }
            }
        }
        else {
            faces.left = (blockType[x - 1][y][z] == Blocks::AIR_BLOCK) && (blockType[x][y][z] != Blocks::AIR_BLOCK);
        }
        if (x == 15) {
            if (m_surroundingChunks[0] != nullptr) {
                if (m_surroundingChunks[0]->BlockPresent(0, y, z)) {
                    Blocks adjacentBlock = m_surroundingChunks[0]->GetBlockType(0, y, z);
                    if (adjacentBlock == Blocks::AIR_BLOCK  && blockType[x][y][z] != Blocks::AIR_BLOCK) {
                        faces.right = true;
                    }
                    else {
                        faces.right = false;
                    }
                }
                else {
                    faces.right = true;
                }
            }
        }
        else {
            faces.right = (blockType[x + 1][y][z] == Blocks::AIR_BLOCK) && (blockType[x][y][z] != Blocks::AIR_BLOCK);
        }

        faces.bottom = (y == 0 || (blockType[x][y - 1][z] == Blocks::AIR_BLOCK) && (blockType[x][y][z] != Blocks::AIR_BLOCK));
        faces.top = (y == 255 || (blockType[x][y + 1][z] == Blocks::AIR_BLOCK) && (blockType[x][y][z] != Blocks::AIR_BLOCK));
        if (z == 15) {
            if (m_surroundingChunks[2] != nullptr) {
                if (m_surroundingChunks[2]->BlockPresent(x, y, 0)) {
                    Blocks adjacentBlock = m_surroundingChunks[2]->GetBlockType(x, y, 0);
                    if (adjacentBlock == Blocks::AIR_BLOCK && blockType[x][y][z] != Blocks::AIR_BLOCK) {
                        faces.front = true;
                    }
                    else {
                        faces.front = false;
                    }
                }
                else {
                    faces.front = true;
                }
            }
        }
        else {
            faces.front = (blockType[x][y][z + 1] == Blocks::AIR_BLOCK) && (blockType[x][y][z] != Blocks::AIR_BLOCK);
        }
        if (z == 0) {
            if (m_surroundingChunks[3] != nullptr) {
                if (m_surroundingChunks[3]->BlockPresent(x, y, 15)) {
                    Blocks adjacentBlock = m_surroundingChunks[3]->GetBlockType(x, y, 15);
                    if (adjacentBlock == Blocks::AIR_BLOCK && blockType[x][y][z] != Blocks::AIR_BLOCK) {
                        faces.back = true;
                    }
                    else {
                        faces.back = false;
                    }
                }
                else {
                    faces.back = true;
                }
            }
        }
        else {
            faces.back = (blockType[x][y][z - 1] == Blocks::AIR_BLOCK) && (blockType[x][y][z] != Blocks::AIR_BLOCK);
        }

        return faces;

    }

    bool isAir(Chunk* chunk, int x, int y, int z) {
        if (chunk && chunk->BlockPresent(x, y, z)) {
            return chunk->GetBlockType(x, y, z) == Blocks::AIR_BLOCK;
        }
        return true; // treat missing neighbor as air
    }

    void SetBlock(int x, int y, int z, Blocks block) {
        m_cubePositions.push_back(glm::vec3(x, y, z));
        blockType[x][y][z] = block;
        cubeGrid[x][y][z] = true;
    }

    void RefreshBlock(int x, int y, int z) {
        int index = x + y * 16 + z * 16 * 256;
    }


    bool cubeGrid[16][256][16] = { false };
    unsigned int blockType[16][256][16] = { 4 };
    std::vector<glm::vec3> m_cubePositions{};
    std::vector<double> m_verticies;
    glm::vec3 m_chunkOrigin{};
    std::array<Chunk*, 4> m_surroundingChunks{};
    bool m_shouldCalcVisibleVertsFlag = true;

    std::vector<float[5]> m_visibleVerts{};

    std::vector<float> m_cachedVisibleVerts{};

    unsigned int m_Vao;
    unsigned int m_Vbo;
};
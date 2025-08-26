#pragma once


/*
Used to create blocks for use in engine.
Three indexes are used to specify the location of the blocks texture within the block texture atlas.
*/
struct Block {
public:
    Block() : m_top_texture_index(-1), m_bottom_texture_index(-1), m_side_texture_index(-1) {};
    Block(int top, int bottom, int side) : m_top_texture_index(top), m_bottom_texture_index(bottom), m_side_texture_index(side) {};
    int GetTopTextureIndex() { return m_top_texture_index; }
    int GetSideTextureIndex() { return m_side_texture_index; }
    int GetBottomTextureIndex() { return m_bottom_texture_index; }
private:
    int m_top_texture_index;
    int m_bottom_texture_index;
    int m_side_texture_index;
};

/*
Predefined blocks for use in engine.
*/
std::vector<Block> blocks{
    Block(0, 2, 3), //Grass
    Block(1, 1, 1), //Stone
    Block(2, 2, 2), //Dirt block
    Block(4, 4, 4), //Wood plank
    Block(0, 0, 0) //AirBlock
};

/*
Used for easier readability within other parts of engine.
*/
enum Blocks {
    GRASS_BLOCK = 0,
    STONE_BLOCK = 1,
    DIRT_BLOCK = 2,
    WOOD_PLANK = 3,
    AIR_BLOCK = 4
};
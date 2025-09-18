#pragma once

struct Position {
	float x, y, z;
};

struct Rotation {
	float x, y, z;
};

struct Scale {
	float x, y, z;
};

struct Transform {
public:
	Transform() : m_position(Position{}), m_rotation(Rotation{}), m_scale(Scale{}) {}
	Transform(Position position, Rotation rotation, Scale scale) : m_position(position), m_rotation(rotation), m_scale(scale) {}
	Position m_position;
	Rotation m_rotation;
	Scale m_scale;
};

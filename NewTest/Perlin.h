#pragma once
#include <cmath>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <random>

struct Perlin {
public:
	static void Init() {
		std::vector<int> perm(256);
		for (int i = 0; i < 256; i++) perm[i] = i;

		std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(perm.begin(), perm.end(), g);

		for (int i = 0; i < 256; i++) {
			m_perm[i] = perm[i];
			m_perm[256 + i] = perm[i];
		}
	}
	static double perlin(double x, double y) {
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;

		x -= floor(x);
		y -= floor(y);

		double u = fade(x);
		double v = fade(y);

		int A = m_perm[X] + Y;
		int B = m_perm[X + 1] + Y;

		double n = lerp(
			lerp(grad(m_perm[A], x, y), grad(m_perm[B], x - 1, y), u),
			lerp(grad(m_perm[A + 1], x, y - 1), grad(m_perm[B + 1], x - 1, y - 1), u),
			v
		);

		return (n + 1.0) * 0.5 * 255.0;
	}

	static double perlinOctave(double x, double y, int octaves, double persistence, double lacunarity) {
		double total = 0.0;
		double frequency = 1.0;
		double amplitude = 1.0;
		double maxValue = 0.0; // for normalization

		for (int i = 0; i < octaves; i++) {
			total += Perlin::perlin(x * frequency, y * frequency) * amplitude;

			maxValue += amplitude;
			amplitude *= persistence;  // controls roughness (0.5 is common)
			frequency *= lacunarity;   // controls zoom (2.0 is common)
		}

		return total / maxValue; // normalized to [-1, 1]
	}
private:

	static double fade(double t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	static double lerp(double a, double b, double t) {
		return a + t * (b - a);
	}

	static double grad(int hash, double x, double y) {
		int h = hash & 7;
		double u = h < 4 ? x : y;
		double v = h < 4 ? y : x;
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}

	static int m_perm[512];
};

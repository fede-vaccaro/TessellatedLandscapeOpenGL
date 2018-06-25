#pragma once
#include "HeightMap.h"

HeightMap::HeightMap(int res, float Hrange, int octaves, int primeIndex, double persistance) {

	this->resolution = res;
	std::vector<heightMapThread*> tt(res);

	// heightmap matrix
	this->coords = (float*)malloc(sizeof(float)*res*res);

	for (int z = 0; z < res; z++) {
		tt[z] = new heightMapThread(this->coords, z, res, Hrange, octaves, primeIndex, persistance);
	}

	threadValues maxMin;
	for (int z = 0; z < res; z++) {
		// gather max and min values from every thread
		maxMin = tt[z]->join();
		if (maxMin.max > max)
			this->max = maxMin.max;
		if (maxMin.min < min)
			this->min = maxMin.min;
	}
}

HeightMap::~HeightMap()
{
	free(coords);
	indices->clear();
}

float * HeightMap::getData()
{
	return coords;
}

std::vector<uint3>* HeightMap::getIndices()
{
	indices = new std::vector<uint3>(resolution*resolution);

	unsigned int i = 0;
	for (int z = 0; z < resolution; z++) {
		for (int x = 0; x < resolution; x++) {
			// Nel frattempo creo gli indici
			if (x + 1 < resolution && z + 1 < resolution) {
				uint3 tri = { i, i + resolution, i + resolution + 1 };
				uint3 tri2 = { i, i + resolution + 1, i + 1 };
				indices->push_back(tri);
				indices->push_back(tri2);
			}
			i++;
		}
	}

	return indices;
}

int HeightMap::getResolution()
{
	return resolution;
}

void HeightMap::saveMap(const char * filename)
{
	_int8* data = (_int8*)malloc(sizeof(_int8)*resolution*resolution);

	//map data in 0-255 range
	//  (- min , + max) -> ( 0, max + min) -> ( 0 , 255)
	float valuex;
	for (int j = 0; j < resolution*resolution; j++) {
		valuex = (coords[j] + min) * 255 / max;
		data[j] = (_int8)valuex;
	}

	//int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
	if (stbi_write_bmp(filename, resolution, resolution, 1, data)) {
		std::cout << "HEIGHT_MAP::Height map completed" << std::endl;
	}
	else {
		std::cout << "HEIGH_MAP::Error occured while saving file" << std::endl;
	}

	free(data);
}

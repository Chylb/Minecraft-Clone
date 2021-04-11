#pragma once

#include <string>
#include <mutex>
#include <functional>

#include "../chunk/ChunkPointers.h"


class World;
class Chunk;

class ChunkWorker
{
public:
	//ChunkWorker(World*)
	std::atomic_bool done = false;
	std::mutex joinMutex;
	std::string name;
	
	void Apply4NeighbourhoodJob(std::function<void(ChunkWritePtr& chunk, ChunkReadPtr& nNorth, ChunkReadPtr& nEast, ChunkReadPtr& nSouth, ChunkReadPtr& nWest)> f);
	
	//static void Apply4NeighbourhoodJob(std::function<void(ChunkReadPtr*, ChunkReadPtr*, ChunkReadPtr*, ChunkReadPtr*, ChunkReadPtr*)> f);
	static void GenerateBlocks(ChunkWritePtr& chunk, ChunkReadPtr& nNorth, ChunkReadPtr& nEast, ChunkReadPtr& nSouth, ChunkReadPtr& nWest);
	static void GenerateMesh(ChunkWritePtr& chunk, ChunkReadPtr& nNorth, ChunkReadPtr& nEast, ChunkReadPtr& nSouth, ChunkReadPtr& nWest);
	
};


#include "ChunkWorker.h"

#include <cassert>
#include <array>
#include <iostream>
#include <queue>

#include "../World.h"

extern World* world;
extern std::queue<ChunkWrapper*> g_chunks_requiring_buffer_update_queue;
extern std::mutex g_job_mutex, g_chunks_requiring_buffer_update_queue_mutex;
extern std::condition_variable g_cv;

void ChunkWorker::Apply4NeighbourhoodJob(std::function<void(ChunkWritePtr&, ChunkReadPtr&, ChunkReadPtr&, ChunkReadPtr&, ChunkReadPtr&)> f)
{
	std::cout << name << " starting... " << std::flush;
	
	//póki nie zrobi siê wszystko
	while(!done.load())
	{
		auto map = world->GetChunksMap();
		
		for(const auto& [pos, chunkWrapper] : map)
		{
			if(!chunkWrapper) continue;

			auto nRawNorth = map[{pos.x, pos.z + 1}];
			auto nRawSouth = map[{pos.x, pos.z - 1}];
			auto nRawWest = map[{pos.x - 1, pos.z}];
			auto nRawEast = map[{pos.x + 1, pos.z}];

			auto chunk = chunkWrapper->GetWritePtr(false);
			if (!chunk) continue;
			
			ChunkReadPtr nNorth = nullptr, nSouth = nullptr, nEast = nullptr, nWest = nullptr;
			bool blocking = false;
			if(nRawNorth)
			{
				nNorth = nRawNorth->GetReadPtr(blocking);
				if (!nNorth) continue;
			}
			if (nRawSouth)
			{
				nSouth = nRawSouth->GetReadPtr(blocking);
				if (!nSouth) continue;
			}
			if (nRawEast)
			{
				nEast = nRawEast->GetReadPtr(blocking);
				if (!nEast) continue;
			}
			if (nRawWest)
			{
				nWest = nRawWest->GetReadPtr(blocking);
				if (!nWest) continue;
			}
			//if not blocking, check if mutexes were acquired with success

			f(chunk, nNorth, nEast, nSouth, nWest);

		}
		//std::cout << name << " waiting for join... " << std::flush;
		std::unique_lock joinLock(joinMutex);
		g_cv.wait(joinLock);
		//std::cout << "Yay.\n";
	}
}

//void ChunkWorker::Apply4NeighbourhoodJob(
//	std::function<void(ChunkReadPtr*, ChunkReadPtr*, ChunkReadPtr*, ChunkReadPtr*, ChunkReadPtr*)> f)
//{
//}

void ChunkWorker::GenerateBlocks(ChunkWritePtr& chunk, ChunkReadPtr& nNorth, ChunkReadPtr& nEast, ChunkReadPtr& nSouth, ChunkReadPtr& nWest)
{
	if (chunk->loadingState != Chunk::LoadingState::loading_blocks) return;
	
	world->PopulateChunk(*chunk);
	chunk->loadingState = Chunk::LoadingState::generating_mesh;
}

void ChunkWorker::GenerateMesh(ChunkWritePtr& chunk, ChunkReadPtr& nNorth, ChunkReadPtr& nEast, ChunkReadPtr& nSouth, ChunkReadPtr& nWest)
{
	if (chunk->loadingState != Chunk::LoadingState::generating_mesh) return;
	if (!nNorth || !nSouth || !nEast || !nWest) return;
	if (nNorth->loadingState < Chunk::LoadingState::loaded_blocks 
		|| nSouth->loadingState < Chunk::LoadingState::loaded_blocks 
		|| nEast->loadingState < Chunk::LoadingState::loaded_blocks 
		|| nWest->loadingState < Chunk::LoadingState::loaded_blocks) return;

	//assert(chunk->adjacentChunks[0] && chunk->adjacentChunks[1] && chunk->adjacentChunks[2] && chunk->adjacentChunks[3]);

	std::array<ChunkReadPtr, 4> adjacentChunks = { { nNorth, nEast, nSouth, nWest } };
	chunk->GenerateMesh(adjacentChunks);
	chunk->loadingState = Chunk::LoadingState::generated_mesh;
	std::lock_guard lock(g_chunks_requiring_buffer_update_queue_mutex);
	//danger zone
	g_chunks_requiring_buffer_update_queue.push((ChunkWrapper*)(chunk.get()));
}

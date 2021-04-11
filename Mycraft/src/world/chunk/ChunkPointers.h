#pragma once

#include <memory>

class Chunk;

void freeChunkReadPtr(Chunk*);
typedef std::shared_ptr<Chunk> ChunkReadPtr;

//struct FreeChunkWritePtr {
//    void operator()(Chunk* chunk);
//};

static void freeChunkWritePtr(Chunk* chunk);
typedef std::shared_ptr<Chunk> ChunkWritePtr;

//typedef std::unique_ptr<Chunk, FreeChunkWritePtr> ChunkWritePtr;

/*
 * Copyright 2012-2016 Moritz Hilscher
 *
 * This file is part of Mapcrafter.
 *
 * Mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHUNK_H_
#define CHUNK_H_

#include "nbt.h"
#include "pos.h"
#include "worldcrop.h"

#include <stdint.h>
#include <unordered_map>

namespace mapcrafter {
namespace mc {

class BlockStateRegistry;

// chunk height in sections
const int CHUNK_LOW = -64/16;
const int CHUNK_TOP = 320/16;
const int BIOMES_ARRAY_SIZE = 16/4 * 16/4 * ((CHUNK_TOP-CHUNK_LOW)*16)/4;

/**
 * A 16x16x16 section of a chunk.
 */
struct ChunkSection {
	int8_t y;
	uint8_t block_light[16 * 16 * 8];
	uint8_t sky_light[16 * 16 * 8];
	uint16_t block_ids[16 * 16 * 16];

	inline const uint8_t* getArray(int index) const {
		if (index == 0) {
			return block_light;
		} else {
			return sky_light;
		}
	}
};

/**
 * This class represents a Minecraft Chunk and provides an read-only interface to chunk
 * data such as block IDs, block data values and block lighting data.
 *
 * To save memory, the class stores only the sections which exist in the NBT data.
 */
class Chunk {
public:
	Chunk();
	~Chunk();

	/**
	 * Sets the rotation of the world. You have to call this before loading the NBT data.
	 */
	void setRotation(int rotation);

	/**
	 * Sets the boundaries of the world.
	 */
	void setWorldCrop(const WorldCrop& world_crop);

	/**
	 * Reads the NBT data of the chunk from a buffer. You need to specify a compression
	 * type of the raw data.
	 */
	bool readNBT(BlockStateRegistry& block_registry, const char* data, size_t len,
			nbt::Compression compression = nbt::Compression::ZLIB);

	/**
	 * Clears all loaded chunk data.
	 */
	void clear();

	/**
	 * Returns whether the chunk has a specific section.
	 */
	bool hasSection(int section) const;

	/**
	 * Returns the block ID at a specific position (local coordinates).
	 */
	uint16_t getBlockID(const LocalBlockPos& pos, bool force = false) const;

	/**
	 * Returns the block light at a specific position (local coordinates).
	 */
	uint8_t getBlockLight(const LocalBlockPos& pos) const;

	/**
	 * Returns the block sky light at a specific position (local coordinates).
	 */
	uint8_t getSkyLight(const LocalBlockPos& pos) const;

	/**
	 * Returns the block light at a specific position (local coordinates).
	 */
	uint8_t getBiomeAt(const LocalBlockPos& pos) const;

	/**
	 * Returns the position of the chunk. This position may be, depending on the map,
	 * the rotated version of the original position.
	 */
	const ChunkPos& getPos() const;

private:
	// internal original chunk position and public chunk position (which may be rotated)
	ChunkPos chunkpos, chunkpos_original;

	// rotation and cropping of the world
	int rotation;
	WorldCrop world_crop;
	// whether the chunk is completely contained (according x- and z-coordinates, not y)
	bool chunk_completely_contained;

	uint16_t air_id;

	// the index of the chunk sections in the sections array
	// or -1 if section does not exist
	int section_offsets[CHUNK_TOP-CHUNK_LOW];
	// the array with the sections, see indexes above
	std::vector<ChunkSection> sections;

	// the biomes in this chunk, as index y * 16 + z * 4 + x
	uint32_t biomes[BIOMES_ARRAY_SIZE];

	// extra_data (e.g. from attributes read from NBT data, like beds) are stored in this map
	std::unordered_map<int, uint16_t> extra_data_map;

	/**
	 * Checks whether a block (local coordinates, original/unrotated) is in the cropped
	 * part of the world and therefore not rendered.
	 */
	bool checkBlockWorldCrop(int x, int z, int y) const;
	
	/**
	 * Returns a specific block data (block data value, block light, sky light) at a
	 * specific position. The parameter array specifies which one:
	 *   0: block light,
	 *   1: sky light
	 */
	uint8_t getData(const LocalBlockPos& pos, int array, bool force = false) const;

	int positionToKey(int x, int z, int y) const;
	void insertExtraData(const LocalBlockPos& pos, uint16_t extra_data);
	uint16_t getExtraData(const LocalBlockPos& pos, uint16_t default_value = 0) const;
};

}
}

#endif /* CHUNK_H_ */

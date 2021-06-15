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

#ifndef TILERENDERER_H_
#define TILERENDERER_H_

#include "biomes.h"
#include "image.h"
#include "../mc/worldcache.h" // mc::DIR_*

#include <array>
#include <vector>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace mapcrafter {

// some forward declarations
namespace mc {
class BlockPos;
class BlockStateRegistry;
class Chunk;
}

namespace renderer {

class BlockImages;
struct BlockImage;
class TilePos;
class RenderedBlockImages;
class RenderMode;
class RenderView;

struct TileImage {
	int x, y;
	RGBAImage image;
	mc::BlockPos pos;
	int z_index;

	bool operator<(const TileImage& other) const;
};

class TileRenderer {
public:
	TileRenderer(const RenderView* render_view, mc::BlockStateRegistry& block_registry,
			BlockImages* images, int tile_width, mc::WorldCache* world, RenderMode* render_mode);
	virtual ~TileRenderer();

	void setRenderBiomes(bool render_biomes);
	void setUsePreblitWater(bool use_preblit_water);
	void setShadowEdges(std::array<uint8_t, 5> shadow_edges);

	virtual void renderTile(const TilePos& tile_pos, RGBAImage& tile);

	virtual int getTileSize() const = 0;
	virtual int getTileWidth() const;
	virtual int getTileHeight() const;

protected:
	void renderBlocks(int x, int y, mc::BlockPos top, const mc::BlockPos& dir, std::set<TileImage>& tile_images);
	virtual void renderTopBlocks(const TilePos& tile_pos, std::set<TileImage>& tile_images) {}

	mc::Block getBlock(const mc::BlockPos& pos, int get = mc::GET_ID);
	uint32_t getBiomeColor(const mc::BlockPos& pos, const BlockImage& block, const mc::Chunk* chunk);

	mc::BlockStateRegistry& block_registry;

	BlockImages* images;
	RenderedBlockImages* block_images;
	int tile_width;
	mc::WorldCache* world;
	mc::Chunk* current_chunk;
	RenderMode* render_mode;

	bool render_biomes;
	bool use_preblit_water;
	// factors for shadow edges:
	// north, south, east, west, bottom
	std::array<uint8_t, 5> shadow_edges;

	// IDs of full water blocks appearing in minecraft worlds
	std::set<uint16_t> full_water_ids;
	// IDs of blocks that can be seen as full water blocks for other full water blocks
	// (for example ice: we don't want side faces of water next to ice)
	std::set<uint16_t> full_water_like_ids;
	// full water blocks will be replaced by these water blocks
	std::vector<uint16_t> partial_full_water_ids, partial_ice_ids;

	std::vector<uint16_t> lily_pad_ids;

	uint16_t waterlog_id;
	const BlockImage* waterlog_block_image;
};

}
}

#endif /* TILERENDERER_H_ */

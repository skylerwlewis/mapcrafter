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

#include "pos.h"

#include "chunk.h"
#include "../util.h"

#include <cmath>
#include <cstdio>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace mapcrafter {
namespace mc {

RegionPos::RegionPos()
	: x(0), z(0) {
}

RegionPos::RegionPos(int x, int z)
	: x(x), z(z) {
}

bool RegionPos::operator==(const RegionPos& other) const {
	return x == other.x && z == other.z;
}

bool RegionPos::operator!=(const RegionPos& other) const {
	return !operator==(other);
}

bool RegionPos::operator<(const RegionPos& other) const {
	if (x == other.x)
		return z < other.z;
	return x < other.x;
}

RegionPos RegionPos::byFilename(const std::string& filename) {
	std::string name = BOOST_FS_FILENAME(fs::path(filename));

	int x, z;
	if (sscanf(name.c_str(), "r.%d.%d.mca", &x, &z) != 2)
		throw std::runtime_error("Invalid filename " + name + "!");
	return RegionPos(x, z);
}

void RegionPos::rotate(int count) {
	for (int i = 0; i < count; i++) {
		int nx = -z, nz = x;
		x = nx;
		z = nz;
	}
}

ChunkPos::ChunkPos()
	: x(0), z(0) {
}

ChunkPos::ChunkPos(int x, int z)
	: x(x), z(z) {
}

ChunkPos::ChunkPos(const BlockPos& block) {
	x = util::floordiv(block.x, 16);
	z = util::floordiv(block.z, 16);
}

int ChunkPos::getLocalX() const {
	return x % 32 < 0 ? x % 32 + 32 : x % 32;
}
int ChunkPos::getLocalZ() const {
	return z % 32 < 0 ? z % 32 + 32 : z % 32;
}

RegionPos ChunkPos::getRegion() const {
	return RegionPos(util::floordiv(x, 32), util::floordiv(z, 32));
}

bool ChunkPos::operator==(const ChunkPos& other) const {
	return x == other.x && z == other.z;
}

bool ChunkPos::operator!=(const ChunkPos& other) const {
	return !operator ==(other);
}

bool ChunkPos::operator<(const ChunkPos& other) const {
	if (x == other.x)
		return z < other.z;
	return x < other.x;
}

int ChunkPos::getRow() const {
	return z - x;
}

int ChunkPos::getCol() const {
	return x + z;
}

ChunkPos ChunkPos::byRowCol(int row, int col) {
	return ChunkPos((col - row) / 2, (col + row) / 2);
}

void ChunkPos::rotate(int count) {
	for (int i = 0; i < count; i++) {
		int nx = 31 - z;
		z = x;
		x = nx;
	}
}

BlockPos::BlockPos()
	: x(0), z(0), y(0) {
}

BlockPos::BlockPos(int x, int z, int y)
	: x(x), z(z), y(y) {
}

int BlockPos::getRow() const {
	return z - x + (CHUNK_TOP*16 - y) * 4;
}

int BlockPos::getCol() const {
	return x + z;
}

BlockPos& BlockPos::operator+=(const BlockPos& p) {
	x += p.x;
	z += p.z;
	y += p.y;
	return *this;
}

BlockPos& BlockPos::operator-=(const BlockPos& p) {
	x -= p.x;
	z -= p.z;
	y -= p.y;
	return *this;
}

BlockPos BlockPos::operator+(const BlockPos& p2) const {
	BlockPos p = *this;
	return p += p2;
}

BlockPos BlockPos::operator-(const BlockPos& p2) const {
	BlockPos p = *this;
	return p -= p2;
}

bool BlockPos::operator==(const BlockPos& other) const {
	return x == other.x && z == other.z && y == other.y;
}

bool BlockPos::operator!=(const BlockPos& other) const {
	return !operator==(other);
}

bool BlockPos::operator<(const BlockPos& other) const {
	if (y == other.y) {
		if (x == other.x)
			return z < other.z;
		return x > other.x;
	}
	return y < other.y;
}

extern const mc::BlockPos DIR_NORTH(0, -1, 0);
extern const mc::BlockPos DIR_SOUTH(0, 1, 0);
extern const mc::BlockPos DIR_EAST(1, 0, 0);
extern const mc::BlockPos DIR_WEST(-1, 0, 0);
extern const mc::BlockPos DIR_TOP(0, 0, 1);
extern const mc::BlockPos DIR_BOTTOM(0, 0, -1);

LocalBlockPos::LocalBlockPos()
	: x(0), z(0), y(0) {
}

LocalBlockPos::LocalBlockPos(int x, int z, int y)
	: x(x), z(z), y(y) {
}

LocalBlockPos::LocalBlockPos(const BlockPos& pos)
		: x(pos.x & 15), z(pos.z & 15), y(pos.y) {
}

int LocalBlockPos::getRow() const {
	return z - x + (CHUNK_TOP*16 - y) * 4;
}

int LocalBlockPos::getCol() const {
	return x + z;
}

BlockPos LocalBlockPos::toGlobalPos(const ChunkPos& chunk) const {
	return BlockPos(x + chunk.x * 16, z + chunk.z * 16, y);
}

bool LocalBlockPos::operator<(const LocalBlockPos& other) const {
	if (y == other.y) {
		if (x == other.x)
			return z < other.z;
		return x > other.x;
	}
	return y < other.y;
}

std::ostream& operator<<(std::ostream& stream, const RegionPos& region) {
	stream << region.x << ":" << region.z;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const ChunkPos& chunk) {
	stream << chunk.x << ":" << chunk.z;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const BlockPos& block) {
	stream << block.x << ":" << block.z << ":" << block.y;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const LocalBlockPos& block) {
	stream << block.x << ":" << block.z << ":" << block.y;
	return stream;
}

}
}

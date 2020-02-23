#ifndef __DIMS_H
#define __DIMS_H

#include "common.h"

struct StatsDims {
    StatsDims() {}
    
    StatsDims(std::vector<hsize_t> statsDims, hsize_t statsSize, int numBins=0, hsize_t partialHistMult=0) :
        statsDims(statsDims),
        statsSize(statsSize),
        histSize(statsSize * numBins),
        partialHistSize(statsSize * numBins * partialHistMult),
        numBins(numBins)
    {
        if (numBins) {
            for (auto d : statsDims) {
                histDims.push_back(d);
            }
            histDims.push_back(numBins);
        }
    }
    
    std::vector<hsize_t> statsDims;
    std::vector<hsize_t> histDims;
    hsize_t statsSize;
    hsize_t histSize;
    hsize_t partialHistSize;
    int numBins;
};

// This is a bit repetitive but I think it's best to define each case explicitly
struct Dims {
    Dims() {}
    
    Dims(hsize_t width, hsize_t height, bool chunked) :
        N(2),
        width(width), height(height), depth(1), stokes(1),
        chunked(chunked),
        standard({height, width}),
        mipMapExtra({}),
        tileDims({TILE_SIZE, TILE_SIZE}),
        statsXY({}, depth * stokes, defaultBins()) // dims, size, bins
    {}
    
    Dims(hsize_t width, hsize_t height, hsize_t depth, bool chunked) :
        N(3),
        width(width), height(height), depth(depth), stokes(1),
        chunked(chunked),
        standard({depth, height, width}),
        swizzled({width, height, depth}),
        mipMapExtra({depth}),
        tileDims({1, TILE_SIZE, TILE_SIZE}),
        statsXY({depth}, depth * stokes, defaultBins()), // dims, size, bins
        statsXYZ({}, stokes, defaultBins(), depth), // dims, size, bins, partial multiplier
        statsZ({height, width}, width * height * stokes) // dims, size
    {}

    Dims(hsize_t width, hsize_t height, hsize_t depth, hsize_t stokes, bool chunked) :
        N(4),
        width(width), height(height), depth(depth), stokes(stokes),
        chunked(chunked),
        standard({stokes, depth, height, width}),
        swizzled({stokes, width, height, depth}),
        mipMapExtra({stokes, depth}),
        tileDims({1, 1, TILE_SIZE, TILE_SIZE}),
        statsXY({stokes, depth}, depth * stokes, defaultBins()), // dims, size, bins
        statsXYZ({stokes}, stokes, defaultBins(), depth), // dims, size, bins, partial multiplier
        statsZ({stokes, height, width}, width * height * stokes) // dims, size
    {}
    
    int defaultBins() {
        return int(std::max(std::sqrt(width * height), 2.0));
    }
    
    bool useChunks() {
        return chunked && TILE_SIZE <= width && TILE_SIZE <= height;
    }
    
    static Dims makeDims(int N, long* dims, bool chunked) {
        if (N == 2) {
            return Dims(dims[0], dims[1], chunked);
        } else if (N == 3) {
            return Dims(dims[0], dims[1], dims[2], chunked);
        } else if (N == 4) {
            return Dims(dims[0], dims[1], dims[2], dims[3], chunked);
        }
    }

    int N;
    hsize_t width, height, depth, stokes;
    bool chunked;
    
    std::vector<hsize_t> standard;
    std::vector<hsize_t> swizzled;
    std::vector<hsize_t> mipMapExtra;
    std::vector<hsize_t> tileDims;
    
    StatsDims statsXY;
    StatsDims statsXYZ;
    StatsDims statsZ;
};

#endif

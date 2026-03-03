// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#ifndef TiffInfo_H
#define TiffInfo_H
#include <cinttypes>
#include <tiffio.h>
#include <string>

class TiffInfo {
public:
  uint32_t width = 0;
  uint32_t height = 0;
  uint16_t bitsPerSample = 0;
  uint16_t samplesPerPixel = 0;
  bool get(TIFF *tif)
  {
    if (!tif) return false;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    return true;
  }  
  bool get(const std::string &ifname)
  {
    TIFF *tif = TIFFOpen(ifname.c_str(), "r");
    if (!tif) return false;
    return get(tif);
  }
};

#endif

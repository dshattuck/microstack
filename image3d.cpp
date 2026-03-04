// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#include <iostream>
#include <image3d.h>
#include <tiffinfo.h>

void Image3D::initialize(const uint32_t w, const uint32_t h, const uint32_t n)
{
  width = w;
  height = h;
  nSlices = n;
  data.resize(width*height*nSlices);
}

bool Image3D::readTiff(const std::string &ifname, uint32_t z)
{
  bool success=false;
  TIFF *tif=TIFFOpen(ifname.c_str(),"r");
  if (!tif) return false;
  TiffInfo info;
  info.get(tif);
  if (info.bitsPerSample!=16||info.samplesPerPixel!=1)
  {
    std::cerr<<"only single-channel 16-bit images are currently supported."<<std::endl;
    return false;
  }
  auto scanlineSize=TIFFScanlineSize(tif);
  if (scanlineSize!=sizeof(uint16_t)*info.width)
  {
    std::cerr<<"mismatch between scanline size ("<<scanlineSize<<") and image width ("<<info.width<<")."<<std::endl;
    TIFFClose(tif);
    return false;
  }
  uint32_t slicesize=width*height;
  for (uint32_t row=0;row<height;row++)
  {
    if (TIFFReadScanline(tif,data.data()+z*slicesize+row*width,row,0)==-1)
    {
      std::cerr<<"error reading scanline "<<row<<" in file "<<ifname<<std::endl;
      break;
    }
  }
  TIFFClose(tif);
  return true;
}

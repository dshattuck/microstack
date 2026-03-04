// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#ifndef H5FileWriter_H
#define H5FileWriter_H

#include <string>
#include <vector>
#include <H5Cpp.h>

class Image2D;
class Image3D;

class H5FileWriter {
public:
  class ImageVar {
  public:
    std::string name;
    std::vector<hsize_t> dims;
    H5::DataSpace dataspace;
    H5::DataSet dataset;
  };
  bool open(std::string fname, const hsize_t nx, const hsize_t ny, const hsize_t nz);
  bool createImageVar(const std::string &varname, hsize_t chunkSize, int compression);
  bool writeSlice(Image2D &image2D, const hsize_t z);
  bool writeSlab(Image3D &image3D, const hsize_t startingZ, const hsize_t nPlanes);
  std::vector<hsize_t> imageDims;
  H5::H5File file;
  ImageVar imageVar;
};

#endif

// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#ifndef Image3D_H
#define Image3D_H

#include <vector>
#include <string>
#include <cinttypes>

class Image3D {
public:
  void initialize(const uint32_t w, const uint32_t h, const uint32_t n);
  bool readTiff(const std::string &ifname, uint32_t z);
  std::vector<uint16_t> data;  
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t nSlices = 0;  
};

#endif

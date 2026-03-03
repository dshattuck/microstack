// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#ifndef Image2D_H
#define Image2D_H

#include <vector>
#include <string>
#include <cinttypes>

class Image2D {
public:
  bool readTiff(const std::string &ifname);
  std::vector<uint16_t> data;
  uint32_t width = 0;
  uint32_t height = 0;
};

#endif

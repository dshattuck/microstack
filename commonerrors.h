// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#ifndef CommonErrors_H
#define CommonErrors_H

class CommonErrors {
public:
  static int cantRead(std::string ifname) { std::cerr<<"error reading "<<ifname<<std::endl; return 1; }
  static int cantWrite(std::string ofname) { std::cerr<<"error writing "<<ofname<<std::endl; return 1; }
  static int incompatibleVolumes(std::string ofname, std::string ofname2) { std::cerr<<"error: "<<ofname<<" and "<<ofname2<<" have different dimensions."<<std::endl; return 1; }
};

#endif

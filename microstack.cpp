// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#include <argparser.h>
#include "image2d.h"
#include <iostream>
#include <timer.h>
#include <h5filewriter.h>
#include <tiffinfo.h>

bool checkFiles(std::vector<std::string> &ifnames)
{
  Timer timer;
  TiffInfo firstImageInfo;    
  firstImageInfo.get(ifnames.front());
  bool okay=true;
  for (int i=1;i<ifnames.size();i++)
  {
    TiffInfo info;
    if (!info.get(ifnames[i]))
    {
      std::cerr<<"unable to read "<<ifnames[i]<<std::endl;
      okay=false;
    }
    if (info.width != firstImageInfo.width || info.width != firstImageInfo.width)
    {
      std::cerr<<"image mismatch for "<<ifnames[i]<<" : "
          <<info.width<<'x'<<info.width<<" does not match "
          <<firstImageInfo.width<<"x"<<firstImageInfo.width
          <<std::endl;
          okay=false;
    }
  }
  timer.stop();
  std::cout<<"scanned tiff info for "<<ifnames.size()<<" images in "<<timer.elapsed()<<std::endl;
  return okay;
}

int main(int argc, char *argv[])
{
  ArgParserBase ap("microstack");
  std::vector<std::string> ifnames;
  std::string ofname;
  ap.description="tiff to hdf5 stacker for 16-bit single-channel microscopy data";
  ap.bindVector("i",ifnames,"TIFF filenames",true);
  ap.bind("o",ofname,"output_hdf5","output hdf5 file",true);
  if (!ap.parseAndValidate(argc,argv))
  {
    return ap.usage();
  }
  Image2D image;
  if (argc<2)
  {
    std::cout<<"usage: "<<std::endl;
    return 0;
  }
  if (!checkFiles(ifnames))
  {
    std::cerr<<"errors occurred while scanning input files... stopping."<<std::endl;
    return 1;
  }
  std::cout<<"all "<<ifnames.size()<<" images have matching dimensions. proceeding with assembly."<<std::endl;
  TiffInfo firstImageInfo;
  firstImageInfo.get(ifnames.front());
  std::cout<<"image volume will be "<<firstImageInfo.width<<'x'<<firstImageInfo.height<<"x"<<ifnames.size()<<std::endl;

  H5FileWriter h5writer;
  h5writer.open(ofname,firstImageInfo.width,firstImageInfo.height,ifnames.size());
  h5writer.createImageVar("stack");

  Timer t0;
  bool okay=true;
  for (int i=0;i<ifnames.size();i++)
  {
    Timer t;
    t.start();
    Image2D image;
    if (!image.readTiff(ifnames[i]))
    {
      std::cerr<<"unable to read "<<ifnames[i]<<std::endl;
      okay=false;
      break;
    }
    t.stop();
    std::cout<<"read "<<ifnames.size()<<" in "<<t.elapsed()<<std::endl;
    h5writer.writeSlice(image,i);
  }
  t0.stop();
  std::cout<<"read tiffs for "<<ifnames.size()<<" images in "<<t0.elapsed()<<std::endl;
  return 0;
}

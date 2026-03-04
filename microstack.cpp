// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#include <argparser.h>
#include <image2d.h>
#include <image3d.h>
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
  std::string imageName="stack";
  bool buffered=false;
  int compression=0;
  size_t chunksize=32;
  ap.description="tiff to hdf5 stacker for 16-bit single-channel microscopy data";
  ap.bindVector("i",ifnames,"TIFF filenames",true);
  ap.bind("o",ofname,"output_hdf5","output hdf5 file",true);
  ap.bind("n",imageName,"image_name","image path in hdf5 file");
  ap.bind("c",chunksize,"chunk_size","size of chunks ");
  ap.bind("z",compression,"compression_level","level for gzip (0=none; 9=maximum)");
  ap.bindFlag("b",buffered,"buffer writes (faster, uses more memory)");
  if (!ap.parseAndValidate(argc,argv))
  {
    return ap.usage();
  }
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
  if (!h5writer.createImageVar(imageName,chunksize,compression))
  {
    std::cerr<<"could not create image array for /"<<imageName<<". stopping."<<std::endl;
    return 1;
  }
  if (buffered)
  {
    Timer t0;
    bool okay=true;
    Image3D image;
    image.initialize(firstImageInfo.width,firstImageInfo.height,chunksize);
    int bufferPlane=0;
    int nplanes=ifnames.size();
    for (size_t z=0;z<nplanes;z+=chunksize)
    {
      int readPlanes=0;
      Timer t0;
      t0.start();
      for (size_t i=0;i<chunksize;i++)
      {
        if (z+i>=nplanes) break;
        Timer t;
        t.start();
        std::cout<<"reading "<<ifnames[z+i]<<"..."<<std::flush;
        if (!image.readTiff(ifnames[z+i],i))
        {
          std::cerr<<"unable to read "<<ifnames[i]<<std::endl;
          okay=false;
          break;
        }
        t.stop();
        std::cout<<"read slice "<<z+i<<" in "<<t.elapsed()<<std::endl;
        readPlanes++;
      }
      if (!okay) break;
      t0.stop();
      std::cout<<"read "<<readPlanes<<" tiff slices in "<<t0.elapsed()<<std::endl;
      if (!h5writer.writeSlab(image,z,readPlanes))
      {
        std::cerr<<"error: unable to write to hdf5 file. stopping."<<std::endl;
        return 1;
      }
    }
    t0.stop();
    std::cout<<"stacked "<<ifnames.size()<<" tiff slices in "<<t0.elapsed()<<std::endl;
  }
  else
  {
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
      std::cout<<"read "<<ifnames[i]<<" in "<<t.elapsed()<<std::endl;
      if (!h5writer.writeSlice(image,i))
      {
        std::cerr<<"error: unable to write to hdf5 file. stopping."<<std::endl;
        return 1;
      }
    }
    t0.stop();
    std::cout<<"read tiffs for "<<ifnames.size()<<" images in "<<t0.elapsed()<<std::endl;
  }
  return 0;
}

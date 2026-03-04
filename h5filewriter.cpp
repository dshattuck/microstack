// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#include <h5filewriter.h>
#include <iostream>
#include <timer.h>
#include <image2d.h>
#include <image3d.h>

bool H5FileWriter::createImageVar(const std::string &varname, hsize_t chunkSize, int compression)
{
  try
  {
    imageVar.name=varname;
    imageVar.dims = imageDims;
    int n=1;
    H5::DSetCreatPropList cparms;
    hsize_t chunkDims[3] ={ std::min(chunkSize,imageDims[0]), std::min(chunkSize,imageDims[1]), std::min(chunkSize,imageDims[2])};
    cparms.setChunk( 3, chunkDims );
    if (compression>0) cparms.setDeflate(compression);
    imageVar.dataspace = H5::DataSpace (imageVar.dims.size(),&imageVar.dims[0]);  
    imageVar.dataset=H5::DataSet(file.createDataSet(varname.c_str(),H5::PredType::STD_I16LE, imageVar.dataspace, cparms));
    std::cout<<"creating "<<imageVar.name<<std::endl;
  }
  catch( H5::Exception error )
  {
    std::cout<<"chunk: "<<chunkSize<<std::endl;
    error.printErrorStack();
    return false;
  }
  return true;
}

bool H5FileWriter::open(std::string ofname, const hsize_t nx, const hsize_t ny, const hsize_t nz)
{
  std::cout<<"opening "<<ofname<<std::endl;
  try
  {
    file = H5::H5File(ofname, H5F_ACC_TRUNC);
  }
  catch( H5::Exception error )
  {
    error.printErrorStack();
    return false;
  }  
  imageDims = { nz, ny, nx };
  return true;
}

bool H5FileWriter::writeSlice(Image2D &image2D, const hsize_t z)
{
  std::vector<hsize_t> memdims = { 1, imageDims[1], imageDims[2] };
  std::vector<hsize_t> offsets = { 0, 0, 0 };
  offsets[0]=z;
  try
  {
    H5::DataSpace memspace = H5::DataSpace (memdims.size(),&memdims[0]);
    H5::DataSpace hdataspace = imageVar.dataset.getSpace();
    hdataspace.selectHyperslab(H5S_SELECT_SET, &memdims[0], &offsets[0]);
    std::cout<<"writing slice "<<z<<"..."<<std::flush;
    Timer t; t.start();
    imageVar.dataset.write(image2D.data.data(), H5::PredType::STD_I16LE, memspace, hdataspace);
    t.stop();
    std::cout<<"wrote. "<<t.elapsed()<<std::endl;
  }
  catch( H5::Exception error )
  {
    error.printErrorStack();
    return false;
  }
  return true;
}

bool H5FileWriter::writeSlab(Image3D &image3D, const hsize_t startingZ, const hsize_t nplanes)
{
  std::vector<hsize_t> memdims = { nplanes, imageDims[1], imageDims[2] };
  std::vector<hsize_t> offsets = { 0, 0, 0 };
  offsets[0]=startingZ;
  try
  {
    H5::DataSpace memspace = H5::DataSpace (memdims.size(),&memdims[0]);
    H5::DataSpace hdataspace = imageVar.dataset.getSpace();
    hdataspace.selectHyperslab(H5S_SELECT_SET, &memdims[0], &offsets[0]);
    std::cout<<"writing slices ["<<startingZ<<","<<startingZ+nplanes-1<<"]..."<<std::flush;
    Timer t; t.start();
    imageVar.dataset.write(image3D.data.data(), H5::PredType::STD_I16LE, memspace, hdataspace);
    t.stop();
    std::cout<<"written in "<<t.elapsed()<<std::endl;
  }
  catch( H5::Exception error )
  {
    error.printErrorStack();
    return false;
  }
  return true;
}

// Copyright (C) 2026 The Regents of the University of California
// Created by David W. Shattuck, Ph.D.
// SPDX-License-Identifier: LGPL-2.1-only
// https://spdx.org/licenses/LGPL-2.1-only.html

#include <h5filewriter.h>
#include <iostream>
#include <timer.h>
#include <image2d.h>

bool H5FileWriter::createImageVar(const std::string &varname, int compression)
{
  imageVar.name=varname;
  imageVar.dims = imageDims;
  int n=1;
  H5::DSetCreatPropList cparms;
  hsize_t chunk_dims[3] ={32, 32, 32};
  cparms.setChunk( 3, chunk_dims );
  if (compression>0) cparms.setDeflate(compression);
  imageVar.dataspace = H5::DataSpace (imageVar.dims.size(),&imageVar.dims[0]);  
  imageVar.dataset=H5::DataSet(file.createDataSet(varname.c_str(),H5::PredType::STD_I16LE, imageVar.dataspace, cparms));
  std::cout<<"creating "<<imageVar.name<<'\t'
  <<imageVar.dataset.getId()
  <<std::endl;
  return true;
}

bool H5FileWriter::open(std::string ofname, const hsize_t nx, const hsize_t ny, const hsize_t nz)
{
  std::cout<<"opening "<<ofname<<std::endl;
  H5::Exception::dontPrint();
  file = H5::H5File(ofname, H5F_ACC_TRUNC);
  imageDims = { nz, ny, nx };
  return 0;
}

int H5FileWriter::writeSlice(Image2D &image2D, const int z)
{
  // H5::Exception::dontPrint();
  std::vector<hsize_t> memdims = { 1, imageDims[1], imageDims[2] };
  std::vector<hsize_t> offsets = { 0, 0, 0 };
  offsets[0]=z;
  try
  {
    H5::DataSpace memspace = H5::DataSpace (memdims.size(),&memdims[0]);
    H5::DataSpace hdataspace = imageVar.dataset.getSpace();
    hdataspace.selectHyperslab(H5S_SELECT_SET, &memdims[0], &offsets[0]);
    std::cout<<"writing "<<z<<"..."<<std::flush;
    Timer t; t.start();
    imageVar.dataset.write(image2D.data.data(), H5::PredType::STD_I16LE, memspace, hdataspace);
    t.stop();
    std::cout<<"wrote. "<<t.elapsed()<<std::endl;
  }
  catch( H5::FileIException error )
  {
    error.printErrorStack();
    return -1;
  }
  // catch failure caused by the DataSet operations
  catch( H5::DataSetIException error )
  {
    error.printErrorStack();
    return -1;
  }
  // catch failure caused by the DataSpace operations
  catch( H5::DataSpaceIException error )
  {
    error.printErrorStack();
    return -1;
  }
  // catch failure caused by the DataSpace operations
  catch( H5::DataTypeIException error )
  {
    error.printErrorStack();
    return -1;
  }
  return 0;
}

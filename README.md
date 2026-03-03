# MicroStack
C++ program for stacking TIFF images into an hdf5 file.

Supports only unsigned 16-bit integer single-channel images.

## Usage
```
usage: microstack -i image1.tif {image2.tif ... imageN.tif} -o imagestack.h5 [options]

where `image1.tif {image2.tif ... imageN.tif}` is your list of tiff files and `imagestack.h5` is the output hdf5 file.

required settings:
-i <arguments>                 TIFF filenames
-o output_hdf5                 output hdf5 file

optional arguments:
--license                      show the license information and quit
-n image_name                  image path in hdf5 file [default: stack]
-c compression_level           level for gzip (0=none; 9=maximum) [default: 0]
```

Currently, microstack is set to use 32x32x32 chunks and store the images in the output file in the location `/stack`. The data path can be changed with the `-n` option.

## Building

### Requirements
* `c++-23` : a C++ compiler that supports `c++-23`
* `libtiff` : `sudo apt install libtiff-dev`
* `hdf5` : `sudo apt install libhdf5-dev hdf5-tools`

### make
Run `make`. Binary will be in `bin/${MACHTYPE}/`, e.g., `bin/x86_64-pc-linux-gnu/microstack26a_x86_64-pc-linux-gnu`.

## Author
MicroStack is developed by [David Shattuck](http://shattuck.bmap.ucla.edu) at UCLA.

## License
MicroStack is released under the [GNU Lesser General Public License v2.1 only license (SPDX-License-Identifier: LGPL-2.1-only)](https://spdx.org/licenses/LGPL-2.1-only.html)

## Acknowledgments
This project is supported by NIH Grant R01-NS121761 (PIs: David Shattuck and Allan MacKenzie-Graham).

#font-bits - libfreetype based font file extraction tool

unicode is not supported now, and will be added if needed.

## Build
```
git clone https://git.coding.net/binarier/font-bits.git
cd font-bits
mkdir build
cd build
cmake ..
make
```

## Usage

use ``` ./font-bits -e > foo.h ``` to generate header file

use ``` ./font-bits > foo.c ``` to generate data file

the data is available as mono bitmap in structure entries.
![CmakeCI](https://github.com/tort-dla-psa/logicsim/workflows/CmakeCI/badge.svg?branch=master&event=push)

# logicsim
Modern c++/qt logic blocks simulator designed as a successor of famous [Logisim](http://www.cburch.com/logisim/), which I absolutely love and recommend to try.

## Prerequisites
* Qt5
* g++8/clang9 or newer
* cmake3

## Build
```bash
git clone https://github.com/tort-dla-psa/logicsim
git submodule init
git submodule update --recursive
mkdir -p logicsim/build
cd logicsim/build
cmake ..
make -j4
```

## TODO
* add multibits inputs in gates
* add tests 
* add examples
* add value input from properties to real gates
* fix camera zoom and moving
* fix gates resizing
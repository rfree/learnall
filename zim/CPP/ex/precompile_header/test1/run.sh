
echo "Precompile..."
g++    -x c++-header  --std=c++11  -O3  lib.hpp

echo "Compile..."
g++      --std=c++11  -O3 a.cpp


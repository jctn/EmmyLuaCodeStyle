mkdir -p build_osx
cd build_osx
cmake ..
make

mkdir -p ../build
cp CodeFormat/CodeFormat ../build/CodeFormatMacos
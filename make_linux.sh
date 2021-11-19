mkdir -p build_linux
cd build_linux
cmake ..
make

mkdir -p ../build
cp CodeFormat/CodeFormat ../build/CodeFormatLinux
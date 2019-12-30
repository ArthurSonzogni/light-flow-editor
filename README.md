Minimal starter project using the
[SMK library](https://github.com/ArthurSonzogni/smk)

- [Build instructions:](#build-instructions-)
  * [Desktop build:](#desktop-build-)
  * [Webassembly build:](#webassembly-build-)
  * [Linux snap build:](#linux-snap-build-)
- [Want more complexe examples?](#want-more-complexe-examples-)

# Build instructions:

## Desktop build:
~~~bash
mkdir build
cd build
cmake ..
make -j3
./main
~~~

## Webassembly build:
~~~bash
mkdir build_emscripten
cd build_emscripten
emcmake cmake ..
make -j3
python -m SimpleHTTPServer
(visit localhost:8000)
~~~

## Linux snap build:
Nothing to do. Upload your game to github and visit https://snapcraft.io/build.

# Want more complexe examples?

Look at real projects:
* [IceMaze](https://github.com/ArthurSonzogni/IceMaze)
* [InTheCube](https://github.com/ArthurSonzogni/InTheCube)
* [pigami](https://github.com/ArthurSonzogni/pigami)

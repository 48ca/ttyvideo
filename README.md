# ttyvideo
A video player that runs in the tty
## Dependencies
ttyvideo requires OpenCV 2 compiled with video codecs. If your package manager doesn't carry a version of OpenCV like this, you're going to need to download, compile, and install its [source code](https://github.com/opencv/opencv "OpenCV source") before you can use ttyvideo.
ttyvideo also uses CMake>=2.8 to generate files used in the installation process.
## Compilation
You can compile ttyvideo with CMake and Make.
```shell
cmake .
make
```

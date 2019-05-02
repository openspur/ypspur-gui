FROM fedora:latest

RUN dnf install -y \
    make \
    mingw64-bzip2-static \
    mingw64-freetype-static \
    mingw64-gcc-c++ \
    mingw64-pcre2-static \
    mingw64-qt5-qmake \
    mingw64-qt5-qtbase-devel \
    mingw64-qt5-qtbase-static \
    mingw64-qt5-qtbase-static \
    mingw64-win-iconv-static \
  && dnf clean all

COPY . /work
WORKDIR /work
RUN mkdir -p build-win32 \
  && mkdir -p install-win32/bin
RUN cd build-win32 \
  && mingw64-qmake-qt5 ../ypspur_gui.pro \
  && make release LFLAGS="-static -mwindows" \
  && cp ./release/ypspur-gui.exe ../install-win32/bin/ \
  && make clean

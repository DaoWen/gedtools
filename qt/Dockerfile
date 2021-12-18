FROM ubuntu:16.04
WORKDIR /tmp
# Install Dependencies
RUN apt-get update && apt-get install build-essential -y && apt-get install wget libfontconfig1-dev libfreetype6-dev libx11-dev libxcursor-dev libxext-dev libxfixes-dev libxft-dev libxi-dev libxrandr-dev libxrender-dev -y
# Install QT
RUN wget https://download.qt.io/archive/qt/4.7/qt-everywhere-opensource-src-4.7.4.tar.gz && gunzip qt-everywhere-opensource-src-4.7.4.tar.gz && tar xvf qt-everywhere-opensource-src-4.7.4.tar
WORKDIR /tmp/qt-everywhere-opensource-src-4.7.4/
# Configure QT
RUN yes | ./configure -qt-gfx-vnc -opensource
RUN make install
RUN apt-get install fonts-wqy-zenhei -y
ENV PATH="/usr/local/Trolltech/Qt-4.7.4/bin:${PATH}"
CMD ["/bin/bash"]

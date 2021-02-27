#
# The line below states we will base our new image on the Latest Official Ubuntu
FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=nointeractive

#
# Environment variables needed for the
# build system
ENV CONAN_USER_HOME "/conan"
ENV TZ=Europe/London

#
# Identify the maintainer of an image
LABEL maintainer="matheusgarcia28@gmail.com"

#
# Update the image to the latest packages
RUN apt-get update --fix-missing && apt-get upgrade -y

# Graphics dependencies
RUN apt-get install pkg-config -y
RUN apt-get install libgl1-mesa-dev -y
RUN apt-get install xorg-dev -y
RUN apt-get install libxcb-render-util0-dev -y
RUN apt-get install libxcb-xkb-dev -y
RUN apt-get install libxcb-icccm4-dev -y
RUN apt-get install libxcb-image0-dev -y
RUN apt-get install libxcb-keysyms1-dev -y
RUN apt-get install libxcb-xinerama0-dev -y
RUN apt-get install libglu1-mesa-dev -y
RUN apt-get install libjack-dev -y
RUN apt-get install libxcb-util-dev -y

#
# Install the necessary conan stuff
RUN apt-get install python3 -y
RUN apt-get install python3-pip -y
RUN mkdir "/conan"
RUN pip3 install --no-cache -q conan
RUN pip3 install conan --upgrade --no-cache
RUN conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
RUN chmod -R 777 "/conan"
RUN pip3 install --no-cache -q gcovr

#
# Build stuff
RUN apt-get install build-essential -y
RUN apt-get install cmake -y

#
# Utilities
RUN apt-get install shellcheck

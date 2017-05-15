#!/bin/sh

# This script serves as a workaround for probonopd/linuxdeployqt#83 in Linux builds.

prefix="$(dirname $(dirname $(readlink -e $0)))"
export LD_LIBRARY_PATH="${prefix}/lib:${LD_LIBRARY_PATH}"
exec "${prefix}/plugins/ffmpeg.bin" $@

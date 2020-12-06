#!/usr/bin/bash

basedir="$(dirname "$(realpath "$0")")"

if [ $# -lt 2 ]; then
  echo "Usage: $0 CODE LFID"
  exit 255
fi

echo "!insertmacro LANGFILE_EXT $2"
if [ -f "$basedir/../translations/$1.ini" ]; then
  sed 's/^/${LangFileString} /;s/=/ /' "$basedir/../translations/$1.ini"
fi

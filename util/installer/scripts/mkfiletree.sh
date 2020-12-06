#!/usr/bin/bash

if [ $# -lt 2 -o \( "$1" != install -a "$1" != uninstall \) -o ! -d "$2" ]; then
  echo "Usage: $0 (install|uninstall) DIRECTORY"
  exit 255
fi

case $1 in
  install)
    find "$2" -mindepth 1 -type d -printf 'CreateDirectory "$INSTDIR\\%P"\n' \
      -o -type f -printf 'File "\0oname=$INSTDIR\\%P" "%p"\n' | tr '/\000' '\\/'
    ;;
  uninstall)
    find "$2" -mindepth 1 -type d -printf 'RMDir "$INSTDIR\\%P"\n' \
      -o -type f -printf 'Delete "$INSTDIR\\%P"\n' | tr '/' '\\' | tac
    ;;
esac

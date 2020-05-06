#!/usr/bin/env bash

trap 'echo -e "\033[1m\033[31m==> ERROR:\033[0m \033[1mCommand failed\033[0m" >&2' ERR
set -eE

msg() {
  echo -e "\033[1m\033[32m==>\033[0m \033[1m$1\033[0m"
}

package_linux() {
  make install INSTALL_ROOT="${PWD}/Pencil2D"

  msg "Creating AppImage..."
  # "Downgrade" the desktop entry to version 1.0
  sed -i "/^Keywords\(\[[a-zA-Z_.@]\+\]\)\?=/d;/^Version=/cVersion=1.0" \
    Pencil2D/usr/share/applications/pencil2d.desktop
  install -Dm755 /usr/bin/ffmpeg Pencil2D/usr/plugins/ffmpeg
  install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner" \
    "Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  local gst_executables="-executable=Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  for plugin in adpcmdec alsa app audioconvert audioparsers audioresample \
      autodetect coreelements gsm id3demux jack mpg123 mulaw playback \
      pulse typefindfunctions wavparse; do
    install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgst${plugin}.so" \
      "Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
    gst_executables="${gst_executables} -executable=Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
  done
  curl -fsSLO https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
  chmod 755 linuxdeployqt-continuous-x86_64.AppImage
  LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/lib/x86_64-linux-gnu/pulseaudio" \
    ./linuxdeployqt-continuous-x86_64.AppImage \
    Pencil2D/usr/share/applications/pencil2d.desktop \
    -executable=Pencil2D/usr/plugins/ffmpeg \
    ${gst_executables} \
    -appimage
  mv Pencil2D*.AppImage* "pencil2d-linux-$(date +%F).AppImage"
  msg "AppImage created"
}

package_osx() {
  msg "Cleaning..."
  make clean
  mv bin Pencil2D
  pushd Pencil2D >/dev/null

  msg "Copying FFmpeg plugin..."
  mkdir Pencil2D.app/Contents/MacOS/plugins
  curl -fsSLo ffmpeg.7z https://evermeet.cx/ffmpeg/getrelease/7z
  curl -fsSLo ffmpeg.7z.sig https://evermeet.cx/ffmpeg/getrelease/7z/sig
  echo "trusted-key 0x476C4B611A660874" > ~/.gnupg/gpg.conf
  curl -fsSL https://evermeet.cx/ffmpeg/0x1A660874.asc | gpg --import
  gpg --verify ffmpeg.7z.sig ffmpeg.7z
  bsdtar xfC ffmpeg.7z Pencil2D.app/Contents/MacOS/plugins
  rm ffmpeg.7z ffmpeg.7z.sig

  msg "Copying required Qt frameworks..."
  macdeployqt Pencil2D.app
  msg "Applying macdeployqt fix..."
  curl -fsSLO https://github.com/aurelien-rainone/macdeployqtfix/archive/master.zip
  bsdtar xf master.zip
  python "${TRAVIS_BUILD_DIR}/build/Pencil2D/macdeployqtfix-master/macdeployqtfix.py" \
    "${TRAVIS_BUILD_DIR}/build/Pencil2D/Pencil2D.app/Contents/MacOS/Pencil2D" \
    /usr/local/Cellar/qt/5.9.1/
  msg "Removing files..."
  rm -rf macdeployqtfix-master master.zip
  popd >/dev/null
  msg "Zipping..."
  bsdtar caf "pencil2d-mac-$(date +%F).zip" Pencil2D
  msg "Zipping done"
}

upload_default() {
  cd "${TRAVIS_BUILD_DIR}/util"

  msg "Starting upload to Google Drive..."
  case "${TRAVIS_OS_NAME}" in
    linux)
      python3 nightly-build-upload.py "${LINUX_NIGHTLY_PARENT}" \
        "${TRAVIS_BUILD_DIR}/build/pencil2d-linux-$(date +%F).AppImage"
      ;;
    osx)
      python3 nightly-build-upload.py "${OSX_NIGHTLY_PARENT}" \
        "${TRAVIS_BUILD_DIR}/build/pencil2d-mac-$(date +%F).zip"
      ;;
  esac
  msg "Upload complete"
}

upload_pr() {
  cd "${TRAVIS_BUILD_DIR}/util"

  msg "Starting upload to Google Drive..."
  case "${TRAVIS_OS_NAME}" in
    linux)
      mv "${TRAVIS_BUILD_DIR}/build/pencil2d-linux-$(date +%F).AppImage" \
        "${TRAVIS_BUILD_DIR}/build/pencil2d-linux-pr-${TRAVIS_PULL_REQUEST}.AppImage"
      python3 pr-build-upload.py "${LINUX_PR_PARENT}" \
        "${TRAVIS_BUILD_DIR}/build/pencil2d-linux-pr-${TRAVIS_PULL_REQUEST}.AppImage"
      ;;
    osx)
      mv "${TRAVIS_BUILD_DIR}/build/pencil2d-mac-$(date +%F).zip" \
        "${TRAVIS_BUILD_DIR}/build/pencil2d-mac-pr-${TRAVIS_PULL_REQUEST}.zip"
      python3 pr-build-upload.py "${OSX_PR_PARENT}" \
        "${TRAVIS_BUILD_DIR}/build/pencil2d-mac-pr-${TRAVIS_PULL_REQUEST}.zip"
      ;;
  esac
  msg "Upload complete"
}

deploy_docs() {
  cd "${TRAVIS_BUILD_DIR}/util/docs"

  msg "Updating online documentation..."
  for i in core svg xmlpatterns; do
    curl -fsSLO "https://doc.qt.io/qt-5/qt${i}.tags"
  done

  ./documentation-deploy.sh
  msg "Documentation updated"
}

"package_${TRAVIS_OS_NAME}"

if [ "${TRAVIS_BRANCH}" = "master" ] || [ "${TRAVIS_BRANCH}" = "release" ] || [ "${FORCE_NIGHTLY_UPLOAD}" = "yes" ]; then
  upload_default
fi

if [ "${TRAVIS_PULL_REQUEST_SLUG}" = "pencil2d/pencil" ]; then
  upload_pr
fi

if [ "${TRAVIS_BRANCH}" = "master" ] && [ "${TRAVIS_OS_NAME}" = "linux" ]; then
  deploy_docs
fi

#!/usr/bin/env bash

trap 'echo "::error::Command failed"' ERR
set -eE

create_package_linux() {
  echo "::group::Set up AppImage contents"
  make install INSTALL_ROOT="${PWD}/Pencil2D"
  echo "::endgroup::"

  echo "::group::Create AppImage"
  # "Downgrade" the desktop entry to version 1.0
  sed -i "/^Keywords\(\[[a-zA-Z_.@]\+\]\)\?=/d;/^Version=/cVersion=1.0" \
    Pencil2D/usr/share/applications/org.pencil2d.Pencil2D.desktop
  install -Dm755 /usr/bin/ffmpeg Pencil2D/usr/plugins/ffmpeg
  install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner" \
    "Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  local gst_executables="-executable=Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  for plugin in adpcmdec alsa app audioconvert audioparsers audioresample \
      autodetect coreelements gsm id3demux jack mpg123 mulaw playback \
      pulse typefindfunctions wavparse apetag; do
    install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgst${plugin}.so" \
      "Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
    gst_executables="${gst_executables} -executable=Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
  done
  curl -fsSLO https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
  chmod 755 linuxdeployqt-continuous-x86_64.AppImage
  local update_info="" # Currently no appimageupdate support for nightly builds
  if [ $IS_RELEASE = "true" ]; then
    update_info="-updateinformation=gh-releases-zsync|${GITHUB_REPOSITORY/\//|}|latest|pencil2d-linux-amd64-*.AppImage.zsync"
  fi
  LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/lib/x86_64-linux-gnu/pulseaudio" \
    ./linuxdeployqt-continuous-x86_64.AppImage \
    Pencil2D/usr/share/applications/org.pencil2d.Pencil2D.desktop \
    -executable=Pencil2D/usr/plugins/ffmpeg \
    ${gst_executables} \
    -extra-plugins=platforms/libqwayland-egl.so,platforms/libqwayland-generic.so,\
platforms/libqwayland-xcomposite-egl.so,platforms/libqwayland-xcomposite-glx.so,\
wayland-decoration-client,wayland-graphics-integration-client,wayland-shell-integration \
    ${update_info} \
    -appimage
  local output_name="pencil2d-linux-$1-$(date +%F)"
  mv Pencil2D*.AppImage "$output_name.AppImage"
  mv Pencil2D*.AppImage.zsync "$output_name.AppImage.zsync" \
    && sed -i '1,/^$/s/^\(Filename\|URL\): .*$/\1: '"$output_name.AppImage/" "$output_name.AppImage.zsync" \
    || true
  echo "::set-output name=output-basename::$output_name"
  echo "::endgroup::"
}

create_package_macos() {
  echo "::group::Clean"
  make clean
  mv bin Pencil2D
  pushd Pencil2D >/dev/null
  echo "::endgroup::"

  echo "::group::Copy FFmpeg plugin"
  mkdir Pencil2D.app/Contents/MacOS/plugins
  curl -fsSLo ffmpeg.7z https://evermeet.cx/ffmpeg/getrelease/7z
  curl -fsSLo ffmpeg.7z.sig https://evermeet.cx/ffmpeg/getrelease/7z/sig
  mkdir -m700 ~/.gnupg
  echo "trusted-key 0x476C4B611A660874" > ~/.gnupg/gpg.conf
  curl -fsSL https://evermeet.cx/ffmpeg/0x1A660874.asc | gpg --import
  gpg --verify ffmpeg.7z.sig ffmpeg.7z
  bsdtar xfC ffmpeg.7z Pencil2D.app/Contents/MacOS/plugins
  rm ffmpeg.7z ffmpeg.7z.sig
  echo "::endgroup::"

  echo "Deploy Qt libraries"
  macdeployqt Pencil2D.app
  echo "::group::Apply macdeployqt fix"
  curl -fsSLO https://github.com/aurelien-rainone/macdeployqtfix/archive/master.zip
  bsdtar xf master.zip
  python macdeployqtfix-master/macdeployqtfix.py \
    Pencil2D.app/Contents/MacOS/Pencil2D \
    /usr/local/Cellar/qt/5.9.1/
  echo "::endgroup::"
  echo "Remove files"
  rm -rf macdeployqtfix-master master.zip
  popd >/dev/null
  echo "Create ZIP"
  bsdtar caf "pencil2d-mac-$1-$(date +%F).zip" Pencil2D
  echo "::set-output name=output-basename::pencil2d-mac-$1-$(date +%F)"
}

create_package_windows() {
  echo "Copy FFmpeg plugin"
  local platform="${INPUT_ARCH%%_*}"
  local ffmpeg="ffmpeg-${platform}.zip"
  curl -fsSLO "https://github.com/pencil2d/pencil2d-deps/releases/download/ffmpge-v4.1.1/$ffmpeg"
  "${WINDIR}\\System32\\tar" xf "${ffmpeg}"
  mkdir bin/plugins
  mv "ffmpeg.exe" bin/plugins/
  rm -rf "${ffmpeg}"

  mv bin Pencil2D
  echo "Remove files"
  find \( -name '*.pdb' -o -name '*.ilk' \) -delete
  echo "::group::Deploy Qt libraries"
  windeployqt Pencil2D/pencil2d.exe
  echo "::endgroup::"
  echo "Copy OpenSSL DLLs"
  local xbits="-x${platform#win}"
  cp "C:\\Program Files\\OpenSSL\\lib"{ssl,crypto}"-1_1${xbits/-x32/}.dll" Pencil2D/
  echo "Create ZIP"
  "${WINDIR}\\System32\\tar" caf "pencil2d-${platform}-$1-$(date +%F).zip" Pencil2D
  echo "::set-output name=output-basename::pencil2d-${platform}-$1-$(date +%F)"
}

"create_package_$(echo $RUNNER_OS | tr '[A-Z]' '[a-z]')" "${GITHUB_RUN_NUMBER}"

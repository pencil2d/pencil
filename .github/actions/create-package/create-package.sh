#!/usr/bin/env bash

trap 'echo "::error::Command failed"' ERR
set -eE

harvest_files() {
  echo "<?xml version='1.0' encoding='utf-8'?>"
  echo "<Wix xmlns='http://wixtoolset.org/schemas/v4/wxs'>"
  echo "  <Fragment>"
  echo "    <ComponentGroup Id='$1' Directory='INSTALLDIR'>"

  while IFS= read -r filepath; do
    local subdirectory="$(dirname "${filepath}")"
    if [ "${subdirectory}" = "." ]; then
      echo "      <Component>"
    else
      echo "      <Component Subdirectory='${subdirectory}'>"
    fi
    echo "        <File Source='${filepath}' />"
    echo "      </Component>"
  done

  echo "    </ComponentGroup>"
  echo "  </Fragment>"
  echo "</Wix>"
}

create_package_linux() {
  echo "::group::Set up AppImage contents"
  ${BUILD_CMD} make -C build install INSTALL_ROOT="/workspace/Pencil2D"
  echo "::endgroup::"

  echo "::group::Create AppImage"
  # "Downgrade" the desktop entry to version 1.0
  ${BUILD_CMD} sed -i "/^Keywords\(\[[a-zA-Z_.@]\+\]\)\?=/d;/^Version=/cVersion=1.0" \
    Pencil2D/usr/share/applications/org.pencil2d.Pencil2D.desktop
  ${BUILD_CMD} install -Dm755 /usr/bin/ffmpeg Pencil2D/usr/plugins/ffmpeg
  ${BUILD_CMD} install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner" \
    "Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  local gst_executables="-executable=Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  for plugin in adpcmdec alsa app audioconvert audioparsers audioresample \
      autodetect coreelements gsm id3demux jack mpg123 mulaw playback \
      pulse typefindfunctions wavparse apetag; do
    ${BUILD_CMD} install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgst${plugin}.so" \
      "Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
    gst_executables="${gst_executables} -executable=Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
  done
  ${BUILD_CMD} curl -fsSLO https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
  ${BUILD_CMD} chmod 755 linuxdeployqt-continuous-x86_64.AppImage
  local update_info="" # Currently no appimageupdate support for nightly builds
  if [ $IS_RELEASE = "true" ]; then
    update_info="-updateinformation=\"gh-releases-zsync|${GITHUB_REPOSITORY/\//|}|latest|pencil2d-linux-amd64-*.AppImage.zsync\""
  fi
  ${BUILD_CMD} bash -c "LD_LIBRARY_PATH=\"\${LD_LIBRARY_PATH}:/usr/lib/x86_64-linux-gnu/pulseaudio\" \
    ./linuxdeployqt-continuous-x86_64.AppImage \
    Pencil2D/usr/share/applications/org.pencil2d.Pencil2D.desktop \
    -executable=Pencil2D/usr/plugins/ffmpeg \
    ${gst_executables} \
    -extra-plugins=platforms/libqwayland-egl.so,platforms/libqwayland-generic.so,\
platforms/libqwayland-xcomposite-egl.so,platforms/libqwayland-xcomposite-glx.so,\
wayland-decoration-client,wayland-graphics-integration-client,wayland-shell-integration \
    ${update_info} \
    -appimage"
  local qtsuffix="-qt${INPUT_QT}"
  local output_name="pencil2d${qtsuffix/-qt5/}-linux-$3"
  mv "${GITHUB_WORKSPACE}"/Pencil2D*.AppImage "$output_name.AppImage"
  mv "${GITHUB_WORKSPACE}"/Pencil2D*.AppImage.zsync "$output_name.AppImage.zsync" \
    && sed -i '1,/^$/s/^\(Filename\|URL\): .*$/\1: '"$output_name.AppImage/" "$output_name.AppImage.zsync" \
    || true
  echo "output-basename=$output_name" >> "${GITHUB_OUTPUT}"
  echo "::endgroup::"
}

create_package_macos() {
  echo "::group::Clean"
  make clean
  mkdir Pencil2D
  mv app/Pencil2D.app Pencil2D/
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

  echo "::group::Deploy Qt libraries"
  macdeployqt Pencil2D.app
  echo "::endgroup::"
  
  popd >/dev/null
  local qtsuffix="-qt${INPUT_QT}"
  local arch=`uname -m`
  local fileinfo="${qtsuffix/-qt5/}-mac-${arch}-$3"
  echo "Create ZIP: pencil2d${fileinfo}.zip"
  ditto -c -k --sequesterRsrc "Pencil2D" "pencil2d${fileinfo}.zip"
  rm -r "Pencil2D"
  echo "output-basename=pencil2d${fileinfo}" > "${GITHUB_OUTPUT}"
}

create_package_windows() {
  echo "::group::Set up application files"
  nmake install INSTALL_ROOT="$(cygpath -w "${PWD}/Pencil2D")"
  echo "::endgroup::"

  echo "Copy FFmpeg plugin"
  local platform="${INPUT_ARCH%%_*}"
  local ffmpeg="ffmpeg-${platform}.zip"
  curl -fsSLO "https://github.com/pencil2d/pencil2d-deps/releases/download/ffmpge-v4.1.1/$ffmpeg"
  "${WINDIR}\\System32\\tar" xf "${ffmpeg}"
  mkdir Pencil2D/plugins
  mv "ffmpeg.exe" Pencil2D/plugins/
  rm -rf "${ffmpeg}"

  echo "Remove files"
  find \( -name '*.pdb' -o -name '*.ilk' \) -delete
  echo "Deploy Qt libraries"
  # windeployqt lists some translation files that it doesn't actually copy, and the MSVC redistributable is handled by the bundle, so skip those
  windeployqt --list relative Pencil2D/pencil2d.exe | grep -v '^translations\\qtbase_' | grep -v '^translations\\qtmultimedia_' | grep -v '^vc_' | harvest_files windeployqt > windeployqt.wxs
  echo "Copy OpenSSL DLLs"
  curl -fsSLO https://download.firedaemon.com/FireDaemon-OpenSSL/openssl-1.1.1w.zip
  "${WINDIR}\\System32\\tar" xf openssl-1.1.1w.zip
  local wordsize="${platform#win}"
  local xbits="x${wordsize}"
  local _xbits="-x${wordsize}"
  cp "openssl-1.1\\${xbits/32/86}\\bin\\lib"{ssl,crypto}"-1_1${_xbits/-x32/}.dll" Pencil2D/
  echo "::group::Create Installer"
  env -C ../util/installer qmake CONFIG-=debug_and_release CONFIG+=release
  env -C ../util/installer "PATH=${PATH/\/usr\/bin:/}" nmake
  env -C Pencil2D find resources/ -type f | harvest_files resources > resources.wxs
  for i in ../util/installer/translations/pencil2d_*.wxl.xlf; do
    local basename="$(basename -s .wxl.xlf "$i")"
    local locale="${basename#*_}"
    local culture="${locale/_/-}"
    local lcid="$(pwsh -c "(Get-Culture -Name ${culture}).LCID")"
    sed "s/Culture=\"en\"/Culture=\"${culture}\"/;s/Language=\"9\"/Language=\"${lcid}\"/" ../util/installer/pencil2d.wxl > "../util/installer/pencil2d_${locale}.wxl"
    tikal.bat -m -fc ../util/installer/okf_xml_wxl -ie utf-8 -oe utf-8 -sd ../util/installer -od ../util/installer "${i}"
  done
  local versiondefines="-d Edition=Nightly -d NightlyBuildNumber=$1 -d NightlyBuildTimestamp=$(date +%F)"
  if [ "$IS_RELEASE" = "true" ]; then
    versiondefines="-d Edition=Release -d Version=$2"
  fi
  wix build -pdbtype none -arch "x${wordsize/32/86}" -dcl high -b ../util/installer -b Pencil2D \
    -d "ProductCode=$(python -c "import uuid; print(str(uuid.uuid5(uuid.NAMESPACE_URL, '-Nhttps://github.com/${GITHUB_REPOSITORY}/commit/${GITHUB_SHA}#${platform}')).upper())")" \
    $versiondefines \
    -out "pencil2d-${platform}-$3.msi" \
    ../util/installer/pencil2d.wxs windeployqt.wxs resources.wxs
  wix build -pdbtype none -arch "x${wordsize/32/86}" -dcl high -sw1133 -b ../util/installer -b Pencil2D \
    -ext WixToolset.Util.wixext -ext WixToolset.BootstrapperApplications.wixext \
    $versiondefines \
    -out "pencil2d-${platform}-$3.exe" \
    ../util/installer/pencil2d.bundle.wxs
  echo "::endgroup::"
  echo "Create ZIP"
  local qtsuffix="-qt${INPUT_QT}"
  "${WINDIR}\\System32\\tar" caf "pencil2d${qtsuffix/-qt5/}-${platform}-$3.zip" Pencil2D
  # This basename pattern deliberately does not include the installer for the Qt 6 build.
  # Should this ever be changed so that more than one installer is uploaded per workflow run,
  # absolutely make sure not to break any Windows Installer rules.
  echo "output-basename=pencil2d${qtsuffix/-qt5/}-${platform}-$3" > "${GITHUB_OUTPUT}"
}

echo "Version: ${VERSION_NUMBER}"

filename_suffix="b${GITHUB_RUN_NUMBER}-$(date +%F)"
if [ "$IS_RELEASE" = "true" ]; then
  filename_suffix="${VERSION_NUMBER}"
fi

"create_package_$(echo $RUNNER_OS | tr '[A-Z]' '[a-z]')" "${GITHUB_RUN_NUMBER}" "${VERSION_NUMBER}" "${filename_suffix}"

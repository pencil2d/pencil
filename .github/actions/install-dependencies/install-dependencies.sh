#!/usr/bin/env bash
set -e

setup_linux() {
  # Because of how bare-bones our docker image is
  echo "::group::Install prerequisites"
  ${BUILD_CMD} apt-get -yq update
  ${BUILD_CMD} apt-get -yq install software-properties-common
  echo "::endgroup::"

  echo "::group::Add APT sources"
  for ppa in ppa:ubuntu-toolchain-r/test ppa:git-core/ppa; do
    ${BUILD_CMD} apt-add-repository -y "${ppa}"
  done
  if [ "${INPUT_QT}" -eq 5 ]; then
    ${BUILD_CMD} apt-add-repository -y ppa:beineri/opt-qt-5.15.2-xenial
  fi
  echo "::endgroup::"

  echo "::group::Fetch APT updates"
  ${BUILD_CMD} apt-get update -yq
  echo "::endgroup::"

  echo "::group::Install APT packages"
  if [ "${INPUT_QT}" -eq 5 ]; then
    ${BUILD_CMD} apt-get install -yq --no-install-suggests --no-install-recommends \
      build-essential qt515tools qt515base qt515multimedia qt515svg \
      qt515wayland libgl1-mesa-dev bsdtar ffmpeg gstreamer1.0-plugins-base \
      gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
      gstreamer1.0-plugins-ugly gstreamer1.0-alsa gstreamer1.0-pulseaudio git \
      curl libfuse2
  else
    ${BUILD_CMD} apt-get install -yq --no-install-suggests --no-install-recommends \
      build-essential qt6-l10n-tools qt6-base-dev qt6-multimedia-dev \
      libqt6svg6-dev qt6-wayland-dev libgl1-mesa-dev libarchive-tools ffmpeg \
      gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
      gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-alsa \
      gstreamer1.0-pulseaudio git curl libfuse2
  fi
  echo "::endgroup::"
}

setup_macos() {
  echo "::group::Update Homebrew"
  brew update
  echo "::endgroup::"
  echo "::group::Install Homebrew packages"
  brew install libarchive qt@${INPUT_QT}
  brew link qt@${INPUT_QT} --force
  echo "/usr/local/opt/libarchive/bin" >> "${GITHUB_PATH}"
  echo "::endgroup::"
}

setup_windows() {
  pip install translate-toolkit[rc]
  echo "Downloading Okapi Framework..."
  curl -fsSL -o okapi-apps_win32-x86_64_1.45.0.zip https://github.com/pencil2d/pencil2d-deps/releases/download/okapi-v1.45.0/okapi-apps_win32-x86_64_1.45.0.zip
  ls -lh okapi-apps_win32-x86_64_1.45.0.zip
  mkdir okapi
  "${WINDIR}\\System32\\tar" xfC okapi-apps_win32-x86_64_1.45.0.zip okapi
  dotnet tool install -g wix --version 5.0.0
  wix extension add -g WixToolset.Util.wixext/5.0.0 WixToolset.BootstrapperApplications.wixext/5.0.0
  nuget install -x -OutputDirectory util/installer WixToolset.DUtil -Version 5.0.0
  nuget install -x -OutputDirectory util/installer WixToolset.BootstrapperApplicationApi -Version 5.0.0
  nuget install -x -OutputDirectory util/installer WixToolset.WixStandardBootstrapperApplicationFunctionApi -Version 5.0.0
}

"setup_$(echo "${RUNNER_OS}" | tr '[A-Z]' '[a-z]')"

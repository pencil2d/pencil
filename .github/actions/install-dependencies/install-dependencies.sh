#!/usr/bin/env bash
set -ex

setup_linux() {
  # Because of how bare-bones our docker image is
  echo "::group::Install prerequisites"
  
  ${BUILD_CMD} dnf -yq check-update || [ $? -eq 100 ]
  ${BUILD_CMD} dnf -yq install epel-release
  if [ "${INPUT_QT}" -eq 5 ]; then
    ${BUILD_CMD} dnf config-manager --set-enabled powertools
    ${BUILD_CMD} dnf install -yq --nogpgcheck https://mirrors.rpmfusion.org/nonfree/el/rpmfusion-nonfree-release-8.noarch.rpm
  else
    ${BUILD_CMD} dnf config-manager --set-enabled crb
    ${BUILD_CMD} dnf install -yq --nogpgcheck https://mirrors.rpmfusion.org/nonfree/el/rpmfusion-nonfree-release-9.noarch.rpm
  fi
  echo "::endgroup::"

  echo "::group::Install packages"
  ${BUILD_CMD} dnf group install -yq "Development Tools"
  if [ "${INPUT_QT}" -eq 5 ]; then
    ${BUILD_CMD} dnf install -yq \
      qt5-qttools-devel qt5-qtbase-devel qt5-qtmultimedia-devel qt5-qtsvg-devel \
      qt5-qtwayland-devel mesa-libGL-devel bsdtar ffmpeg \
      gstreamer1 gstreamer1-plugins-base gstreamer1-plugins-good \
      gstreamer1-plugins-base gstreamer1-plugins-good \
      gstreamer1-plugins-bad-free gstreamer1-plugins-ugly \
      curl fuse-libs
  else
    ${BUILD_CMD} dnf install -yq \
      qt6-linguist qt6-qtbase-devel qt6-qtmultimedia-devel \
      qt6-qtsvg-devel qt6-qtwayland-devel mesa-libGL-devel bsdtar ffmpeg \
      gstreamer1 gstreamer1-plugins-base gstreamer1-plugins-good \
      gstreamer1-plugins-bad-free gstreamer1-plugins-ugly \
      curl-minimal fuse-libs patchelf
  fi
  echo "::endgroup::"
}

setup_macos() {
  echo "::group::Update Homebrew"
  brew update
  echo "::endgroup::"
  echo "::group::Install Homebrew packages"
  brew install libarchive
  # For Qt 6, we use aqtinstall (jurplel/install-qt-action) instead of Homebrew
  # because Homebrew's Qt 6 arm64 version is problematic
  if [ "${INPUT_QT}" -eq 5 ]; then
    brew install qt@${INPUT_QT}
    brew link qt@${INPUT_QT} --force
  fi
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
  dotnet tool install -g wix --version 6.0.2
  wix extension add -g WixToolset.Util.wixext/6.0.2 WixToolset.BootstrapperApplications.wixext/6.0.2
  nuget install -x -OutputDirectory util/installer WixToolset.DUtil -Version 6.0.2
  nuget install -x -OutputDirectory util/installer WixToolset.BootstrapperApplicationApi -Version 6.0.2
  nuget install -x -OutputDirectory util/installer WixToolset.WixStandardBootstrapperApplicationFunctionApi -Version 6.0.2
}

"setup_$(echo "${RUNNER_OS}" | tr '[A-Z]' '[a-z]')"

#!/usr/bin/env bash

setup_linux() {
  # Because of how bare-bones our docker image is
  echo "::group::Install prerequisites"
  apt-get -yq install software-properties-common
  echo "::endgroup::"

  echo "::group::Add APT sources"
  for ppa in ppa:ubuntu-toolchain-r/test ppa:ubuntu-sdk-team/ppa \
             ppa:git-core/ppa ppa:beineri/opt-qt-5.15.2-xenial; do
    apt-add-repository -y "${ppa}"
  done
  echo "::endgroup::"

  echo "::group::Fetch APT updates"
  apt-get update -yq
  echo "::endgroup::"

  echo "::group::Install APT packages"
  apt-get install -yq --no-install-suggests --no-install-recommends \
    build-essential qt515tools qt515base qt515multimedia qt515svg \
    qt515xmlpatterns qt515wayland libgl1-mesa-dev bsdtar ffmpeg \
    gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-alsa \
    gstreamer1.0-pulseaudio git curl libfuse2 python3 python3-pip
  echo "::endgroup::"

  echo "::group::Install Python packages"
  pip3 install --upgrade oauth2client google-api-python-client typing 'setuptools<44'
  echo "::endgroup::"
}

setup_macos() {
  echo "::group::Update Homebrew"
  brew update
  echo "::endgroup::"
  echo "::group::Install Homebrew packages"
  brew install libarchive qt@5
  brew link qt@5 --force
  echo "/usr/local/opt/libarchive/bin" >> "${GITHUB_PATH}"
  echo "::endgroup::"

  echo "::group::Install Python packages"
  sudo pip3 install --upgrade oauth2client google-api-python-client
  echo "::endgroup::"
}

setup_windows() {
  # Only 64 bit OpenSSL comes pre-installed
  if [ "${INPUT_ARCH%%_*}" = "win32" ]; then
    echo "::group::Install OpenSSL"
    choco install openssl.light --force --x86
    echo "::endgroup::"
  fi

  echo "::group::Install Python packages"
  pip install --upgrade oauth2client google-api-python-client
  echo "::endgroup::"
}

"setup_$(echo "${RUNNER_OS}" | tr '[A-Z]' '[a-z]')"

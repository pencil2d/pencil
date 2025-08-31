#!/usr/bin/env bash

setup_linux() {
  echo "MAKEFLAGS=-j2" >> "${GITHUB_WORKSPACE}/env"
  # Our container image uses the non-Unicode C locale by default
  echo "LANG=C.UTF-8" >> "${GITHUB_WORKSPACE}/env"
  # Required for linuxdeploy-plugin-qt
  echo "QMAKE=/usr/bin/qmake6" >> "${GITHUB_WORKSPACE}/env"
  # Set up Qt environment variables and export them to the GitHub Actions workflow
  ${BUILD_CMD} bash -c 'if [ -f /opt/qt515/bin/qt515-env.sh ]; then (printenv; (. /opt/qt515/bin/qt515-env.sh; printenv)); fi' | sort -st= -k1,1 | uniq -u >> "${GITHUB_WORKSPACE}/env"
}

setup_macos() {
  echo "MAKEFLAGS=-j3" >> "${GITHUB_ENV}"
}

setup_windows() {
  echo "${JAVA_HOME_17_X64}\\bin" >> "${GITHUB_PATH}"
  realpath okapi/ >> "${GITHUB_PATH}"
}

"setup_$(echo "${RUNNER_OS}" | tr '[A-Z]' '[a-z]')"

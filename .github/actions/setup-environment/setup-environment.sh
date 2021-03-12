#!/usr/bin/env bash

setup_linux() {
  echo "MAKEFLAGS=-j2" >> "${GITHUB_ENV}"
  # Our container image uses the non-Unicode C locale by default
  echo "LANG=C.UTF-8" >> "${GITHUB_ENV}"
  # Set up Qt environment variables and export them to the GitHub Actions workflow
  (printenv; (. /opt/qt59/bin/qt59-env.sh; printenv)) | sort -st= -k1,1 | uniq -u >> "${GITHUB_ENV}"
}

setup_macos() {
  echo "MAKEFLAGS=-j2" >> "${GITHUB_ENV}"
}

setup_windows() {
  # Set up MSVC environment variables and export them to the GitHub Actions workflow
  local platform="${INPUT_ARCH%%_*}"
  local vcvars="C:\\Program^ Files^ ^(x86^)\\Microsoft^ Visual^ Studio\\2019\\Enterprise\\VC\\Auxiliary\\Build\\vcvars${platform#win}.bat"
  ($(which cmd) //c set; $(which cmd) //c "${vcvars} 2>&1>nul && set") | sort -st= -k1,1 | uniq -u >> "${GITHUB_ENV}"
}

"setup_$(echo "${RUNNER_OS}" | tr '[A-Z]' '[a-z]')"

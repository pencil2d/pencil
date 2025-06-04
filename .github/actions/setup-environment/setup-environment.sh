#!/usr/bin/env bash

setup_linux() {
  echo "MAKEFLAGS=-j2" >> "${GITHUB_WORKSPACE}/env"
  # Our container image uses the non-Unicode C locale by default
  echo "LANG=C.UTF-8" >> "${GITHUB_WORKSPACE}/env"
  # Set up Qt environment variables and export them to the GitHub Actions workflow
  ${BUILD_CMD} bash -c 'if [ -f /opt/qt515/bin/qt515-env.sh ]; then (printenv; (. /opt/qt515/bin/qt515-env.sh; printenv)); fi' | sort -st= -k1,1 | uniq -u >> "${GITHUB_WORKSPACE}/env"
}

setup_macos() {
  echo "MAKEFLAGS=-j3" >> "${GITHUB_ENV}"
}

setup_windows() {
  # Set up MSVC environment variables and export them to the GitHub Actions workflow
  local platform="${INPUT_ARCH%%_*}"
  local vcvars="C:\\Program^ Files^ ^(x86^)\\Microsoft^ Visual^ Studio\\2019\\Enterprise\\VC\\Auxiliary\\Build\\vcvars${platform#win}.bat"
  
  # Get the environment differences
  local env_diff=$($(which cmd) //c set; $(which cmd) //c "${vcvars} 2>&1>nul && set") | sort -st= -k1,1 | uniq -u)
  
  # Process each environment variable with proper multiline format
  while IFS='=' read -r var_name var_value; do
    if [[ -n "$var_name" && -n "$var_value" ]]; then
      {
        echo "${var_name}<<EOF"
        echo "$var_value"
        echo "EOF"
      } >> "$GITHUB_ENV"
    fi
  done <<< "$env_diff"

  echo "${JAVA_HOME_17_X64}\\bin" >> "${GITHUB_PATH}"
  realpath okapi/ >> "${GITHUB_PATH}"
}

"setup_$(echo "${RUNNER_OS}" | tr '[A-Z]' '[a-z]')"

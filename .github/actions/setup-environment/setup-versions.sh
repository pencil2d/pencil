#!/usr/bin/env bash

# set env.VERSION_NUMBER from release branch names e.g., release/0.7.0
if [[ "${GITHUB_REF_NAME}" == "release/"* ]]; then
  branchVerion=$(echo "${GITHUB_REF_NAME}" | sed 's/release\///')
  VERSION_NUMBER="$branchVerion.${GITHUB_RUN_NUMBER}"
  echo "VERSION_NUMBER=$VERSION_NUMBER" >> $GITHUB_ENV
  echo "VERSION_NUMBER is $VERSION_NUMBER"
fi

# set env.VERSION_NUMBER to 99.0.0.buildNumber if the branch is master
if [ "${GITHUB_REF_NAME}" = "master" ]; then
  VERSION_NUMBER=99.0.0.${GITHUB_RUN_NUMBER}
  echo "VERSION_NUMBER=${VERSION_NUMBER}" >> "${GITHUB_ENV}"
  echo "VERSION_NUMBER is $VERSION_NUMBER"
fi

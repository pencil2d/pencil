#!/usr/bin/env bash

# Check if this is a tag build
if [ "${GITHUB_REF_TYPE}" = "tag" ]; then
  # For tag builds, use the tag name as the version (keep 'v' prefix)
  VERSION_NUMBER="${GITHUB_REF_NAME}"
  echo "VERSION_NUMBER=${VERSION_NUMBER}" >> "${GITHUB_ENV}"
  echo "Tag build detected. VERSION_NUMBER is $VERSION_NUMBER"
elif [[ "${GITHUB_REF_NAME}" == "release/"* ]]; then
  # set env.VERSION_NUMBER from release branch names e.g., release/0.7.0
  branchVerion=$(echo "${GITHUB_REF_NAME}" | sed 's/release\///')
  VERSION_NUMBER="$branchVerion-b${GITHUB_RUN_NUMBER}"
  echo "VERSION_NUMBER=$VERSION_NUMBER" >> $GITHUB_ENV
  echo "Release branch build detected. VERSION_NUMBER is $VERSION_NUMBER"
elif [ "${GITHUB_REF_NAME}" = "master" ]; then
  # set env.VERSION_NUMBER to 99.0.0.buildNumber if the branch is master
  VERSION_NUMBER=99.0.0.${GITHUB_RUN_NUMBER}
  echo "VERSION_NUMBER=${VERSION_NUMBER}" >> "${GITHUB_ENV}"
  echo "Master branch build detected. VERSION_NUMBER is $VERSION_NUMBER"
else
  echo "Branch build detected for branch: ${GITHUB_REF_NAME}"
fi

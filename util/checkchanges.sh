#!/bin/bash

set -e

if [[ $branch == "release" ]]; then
  exit 0
fi

commit=$TRAVIS_COMMIT
branch=$TRAVIS_BRANCH
commit_range=$TRAVIS_COMMIT_RANGE

echo "branch is: $branch"

printf "git diff --name-only %s\n" "${commit_range}"
CHANGED_FILES=`git diff --name-only ${commit_range}`

printf "%s\n" "${CHANGED_FILES}"
printf "=============\n"

ONLY_READMES=True
MD=".md"
SH=".sh"
PY=".py"
PS1=".ps1"

for CHANGED_FILE in $CHANGED_FILES; do
  #printf "Check ${CHANGED_FILES}\n"
  if ! [[ $CHANGED_FILE =~ $MD ||
          $CHANGED_FILE =~ $SH ||
          $CHANGED_FILE =~ $PY ||
          $CHANGED_FILE =~ $PS1 ]] ; then
    ONLY_READMES=False
    printf "Gotcha! %s\n" "${CHANGED_FILE}"
    break
  fi
done

if [[ $ONLY_READMES == True ]]; then
  echo "Only non source code files found, exiting."
  exit 1
else
  echo "source code changes found, continuing with build."
fi

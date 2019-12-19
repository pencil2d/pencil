#!/bin/bash

set -e

git remote set-branches --add origin master
git fetch

commit=$TRAVIS_COMMIT
branch=$TRAVIS_BRANCH
CHANGED_FILES=`git diff --name-only origin/$branch...${commit}`

if [[ $branch == "master" ]]; then
  CHANGED_FILES=`git diff --name-only HEAD^ HEAD`
fi

echo "branch is: $branch"

ONLY_READMES=True
MD=".md"
SH=".sh"
PY=".py"
PS1=".ps1"

printf "git diff --name-only origin/$branch...%s\n" "${TRAVIS_COMMIT}"
printf "%s\n" "${CHANGED_FILES}"
printf "=============\n"

for CHANGED_FILE in $CHANGED_FILES; do
  printf "Check ${CHANGED_FILES}"
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

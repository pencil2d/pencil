#!/bin/bash

set -e

git remote set-branches --add origin master
git fetch
CHANGED_FILES=`git diff --name-only origin/master...${TRAVIS_COMMIT}`
ONLY_READMES=True
MD=".md"
YAML=".yml"
SH=".sh"
PY=".py"
PS1=".ps1"

printf "git diff --name-only origin/master...${TRAVIS_COMMIT}"
printf ${CHANGED_FILES}
printf "==============="

for CHANGED_FILE in $CHANGED_FILES; do
  if ! [[ $CHANGED_FILE =~ $MD || 
  		  $CHANGED_FILE =~ $YAML || 
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
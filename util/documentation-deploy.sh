#!/usr/bin/env bash

trap 'echo -e "\033[1m\033[31m==> ERROR:\033[0m \033[1mCommand failed\033[0m" >&2' ERR
set -eE

msg() {
  echo -e "\033[1m\033[32m==>\033[0m \033[1m$1\033[0m"
}

fetch_tags() {
  pushd "util/docs" 2>&1>/dev/null

  msg "Fetching tag files..."
  for i in core gui network svg testlib widgets xml xmlpatterns; do
    curl -fsSLO "https://doc.qt.io/qt-5/qt${i}.tags"
  done

  popd 2>&1>/dev/null
}

generate_documentation() {
  msg "Generating documentation..."
  cp Doxyfile Doxyfile-Travis;
  echo "PROJECT_NUMBER = ${TRAVIS_COMMIT}" >> Doxyfile-Travis;
  echo "HTML_HEADER = util/docs/header.html" >> Doxyfile-Travis;
  echo "HTML_EXTRA_STYLESHEET = util/docs/extra.css" >> Doxyfile-Travis;
  doxygen Doxyfile-Travis 2>&1 | tee doxygen.log
  test -f docs/html/index.html
}

upload_documentation() {
  msg "Uploading documentation..."
  git clone --depth 1 "https://github.com/${target_repo}.git" docs-repo
  git -C docs-repo config user.name "Travis CI"
  git -C docs-repo config user.email "deploy@travis-ci.org"

  mv doxygen.log docs-repo/
  mv docs-repo/docs/CNAME docs/html/
  rm -rf docs-repo/docs
  mv docs/html docs-repo/docs

  git -C docs-repo add --all
  git -C docs-repo commit -m "Add generated docs for rev ${TRAVIS_REPO_SLUG}@${TRAVIS_COMMIT}" -m "Generated in Travis job #${TRAVIS_JOB_NUMBER}"
  git -C docs-repo push "https://${GH_REPO_TOKEN}@github.com/${target_repo}.git" 2>&1>/dev/null
  msg "Documentation updated"
}

if [ $# -lt 1 ]; then
  echo "Usage: $0 [target repo]" >&2
  exit 1
fi
target_repo="$1"

fetch_tags
generate_documentation
upload_documentation

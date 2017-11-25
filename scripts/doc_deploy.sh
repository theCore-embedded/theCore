#!/usr/bin/env sh
# Pushes documentation to the gh pages

set -e # Stop on error

BRANCH_NAME="travis-${TRAVIS_BUILD_NUMBER}-${TRAVIS_COMMIT}"

cd build
mkdir gh_pages
cd gh_pages
git clone https://${GH_TOKEN}@github.com/forGGe/forGGe.github.io.git . > /dev/null 2>&1

git config --local user.email "travis@travis-ci.org"
git config --local user.name "Travis CI"

# Delete all pages before adding new. Handles case when page should be deleted.
rm -rv ../gh_pages/*
cp -rv ../docs/sphinx/* .

if [ -z $(git status --porcelain) ]; then
    echo "No documentation changes present. Skipping doc deploy"
    return
fi

git checkout -b ${BRANCH_NAME}
git add .
git commit -m 'Travis update from ${TRAVIS_BUILD_NUMBER}'
git push --quiet origin ${BRANCH_NAME}

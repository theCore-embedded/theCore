#!/usr/bin/env sh
# Pushes documentation to the gh pages

set -e # Stop on error
set -v

echo "Travis PR flag: $TRAVIS_PULL_REQUEST"
echo "Travis branch: $TRAVIS_BRANCH"
echo "Travis commit: $TRAVIS_COMMIT"
echo "Travis build number: $TRAVIS_BUILD_NUMBER"


if [ "${TRAVIS_PULL_REQUEST}" != "false" -o "${TRAVIS_BRANCH}" != "develop" ]; then
    echo "Skipping doc deploy. Only develop branch should be deployed"
    exit 0
fi

BRANCH_NAME="travis-${TRAVIS_BUILD_NUMBER}-${TRAVIS_COMMIT}"

cd build
mkdir -p gh_pages
cd gh_pages
git clone https://${GH_TOKEN}@github.com/forGGe/forGGe.github.io.git .

git config --local user.email "travis@travis-ci.org"
git config --local user.name "Travis CI"

# Delete all pages before adding new. Handles case when page should be deleted.
rm -rv ../gh_pages/theCore/*
cp -rv ../docs/sphinx/* .

if [ -z "$(git status --porcelain)" ]; then
    echo "No documentation changes present. Skipping doc deploy"
    exit 0
fi

git checkout -b ${BRANCH_NAME}
git add .
git commit -m 'Travis update from ${TRAVIS_BUILD_NUMBER}'
git push --quiet origin ${BRANCH_NAME}

#!/bin/sh

# Fetches thirdparty dependecy into local cache and advertises them into
# the provided directory using git worktree

set -e

CACHE_THIRDPARTY_DIR="$1"
TARGET_DIR="$2"
GIT_REMOTE="$3"
GIT_COMMIT="$4"

if [ ! -d "$CACHE_THIRDPARTY_DIR" ]; then
    mkdir -vp "$CACHE_THIRDPARTY_DIR"
fi

if [ ! -d "$TARGET_DIR" ]; then
    mkdir -vp "$TARGET_DIR"
fi

cd $CACHE_THIRDPARTY_DIR

if [ ! -d "$CACHE_THIRDPARTY_DIR/.git" ]; then
    echo "git repository does not exist, initializing..."
    git init .
fi

# Check if commit is present or not
if ! git cat-file -e $GIT_COMMIT; then
    echo "no ${GIT_COMMIT} in the thirdparty cache, fething..."
    git fetch ${GIT_REMOTE}
fi

if [ ! -f "$TARGET_DIR/.git" ]; then
    echo "initializing working tree..."

    # In case  Working copy should not exist too. If thats not a case,
    # do not touch any files in TARGET_DIR.
    # They should be manually deleted by an user
    git worktree add ${TARGET_DIR} $GIT_COMMIT
else
    echo "working tree present, checkouting to $GIT_COMMIT"

    cd $TARGET_DIR
    git checkout $GIT_COMMIT
fi

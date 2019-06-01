#!/bin/bash
# Git pull 强制覆盖本地文件

git fetch --all
git reset --hard origin/master
git pull
git fetch origin master
git merge origin/master
git merge origin/master --allow-unrelated-histories
git diff


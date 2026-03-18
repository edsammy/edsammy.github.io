#!/bin/bash
set -e

rm -rf public
hugo

cd public
echo ".DS_Store" > .gitignore
git init
git checkout -b gh-pages
git add --all
git commit -m "Deploy $(date)"
git push --force git@github.com:edsammy/edsammy.github.com.git gh-pages
cd ..

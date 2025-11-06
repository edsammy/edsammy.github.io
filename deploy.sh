#!/bin/bash
set -e
hugo
cd public
if [[ -n $(git status --porcelain) ]]; then
  git add --all
  git commit -m "Deploy $(date)"
  git push origin gh-pages
else
  echo "No changes to deploy."
fi
cd ..

edsammy.github.io
==================
This is the portfolio site of Eddie Samuels

[eddiesamuels.com](https://eddiesamuels.com)

### Deployment Setup
```
# Create an orphan gh-pages branch (no history)
git checkout --orphan gh-pages
git reset --hard
echo "This branch is used for publishing the site." > README.md
git add README.md
git commit -m "Initial gh-pages commit"
git push origin gh-pages

# Go back to your main branch
git checkout master

# Link the gh-pages branch as a worktree in /public
rm -rf public
git worktree add -B gh-pages public origin/gh-pages
```

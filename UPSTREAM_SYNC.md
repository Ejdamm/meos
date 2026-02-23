# Upstream Sync Guide

This repository (`Ejdamm/meos`) is a fork of [`melinsoftware/meos`](https://github.com/melinsoftware/meos).

## Remote Setup

| Remote     | URL                                            |
|------------|------------------------------------------------|
| `origin`   | `git@github.com:Ejdamm/meos.git`              |
| `upstream` | `https://github.com/melinsoftware/meos.git`    |

If `upstream` is missing, add it:
```bash
git remote add upstream https://github.com/melinsoftware/meos.git
```

## Branch Model

- **`master`** — our main branch with custom changes.
- **`upstream/master`** — tracks the original repo (read-only, updated via `git fetch upstream`).
- **Feature branches** — all custom work is done in feature branches merged to `master` via PR.

## Syncing Upstream Changes

### Full merge (recommended for staying up to date)

```bash
git fetch upstream
git checkout -b sync/upstream-YYYY-MM-DD master
git merge upstream/master
# Resolve any conflicts
git push origin sync/upstream-YYYY-MM-DD
# Open PR against master
```

Use merge (not rebase) to preserve clear history of what comes from upstream vs our own changes.

### Cherry-pick (for individual fixes only)

```bash
git fetch upstream
git checkout -b fix/description master
git cherry-pick <commit-sha>
git push origin fix/description
# Open PR against master
```

## Notes

- The fork was created from upstream commit `79d019d` (MeOS 5.0 Beta 1, 2026-02-20).
- Upstream also has a `develop` branch — monitor it for early access to upcoming changes.

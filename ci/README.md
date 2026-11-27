# CI workflow — staged here, not yet active

`github-actions-ci.yml` in this directory is the Milestone A0 CI workflow. It is **not active**: GitHub only runs workflows stored in `.github/workflows/`.

## Why it is here

The push that delivered A0 was rejected by GitHub:

```
refusing to allow an OAuth App to create or update workflow `.github/workflows/ci.yml`
without `workflow` scope
```

The credential available to the implementation agent had `repo` scope but not `workflow`. Rather than drop an authorized A0 deliverable or leave it uncommitted, the file is committed here so it is reviewable and cannot be lost.

## How to activate it

A maintainer with the `workflow` scope runs:

```bash
gh auth refresh -h github.com -s workflow     # one-time, browser device flow
git mv ci/github-actions-ci.yml .github/workflows/ci.yml
git rm ci/README.md
git commit -m "ci: activate Milestone A0 workflow"
git push
```

No edit to the workflow content is required — only its location.

## Status of the workflow itself

Never executed. It has not been observed green, and CI results must not be assumed. See `docs/BUILD_AND_TOOLCHAIN_RECORD.md` §6.

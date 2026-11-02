# Verify a Pencil2D download

CI builds containing this change publish a `<package>.sha256` file alongside
each final application package. Successful builds from `master`, `release/*`,
and tags also publish GitHub build attestations and a portable
`pencil2d-provenance.sigstore.json` bundle. Pull request and ordinary feature
branch builds have checksums only.

Older downloads do not gain attestations retroactively. If no attestation is
available, verification has not succeeded. Choose a build that publishes one,
or use an independently authenticated installation method.

## Check the downloaded bytes

Download the package and its matching `.sha256` from the same completed
GitHub Actions run. Keep both files in the same directory with their original
filenames. The package checksum covers the final archive, installer, DMG, or
AppImage after CI signing and notarization, wherever those are configured.

Using Python 3 and this repository's verification utility:

```sh
python3 util/download_verification.py verify /path/to/package.AppImage.sha256
```

On Windows, use `python` instead of `python3`. The utility returns a nonzero
exit code for a changed or missing file and rejects malformed checksum files.

Alternatively, in the download directory:

```sh
# Linux
sha256sum --check package.AppImage.sha256
# macOS
shasum -a 256 --check package.zip.sha256
```

PowerShell users can compare `(Get-FileHash .\package.zip -Algorithm SHA256).Hash`
with the first 64 characters of `package.zip.sha256`.

A checksum alone detects changed bytes relative to that checksum file. It
does not authenticate the publisher: an attacker could replace both files.

## Authenticate the build

Install a current [GitHub CLI](https://cli.github.com/) and verify the actual
package, using the repository and workflow identity you expect. For an official
tagged release, replace `vX.Y.Z` and the package path:

```sh
gh attestation verify ./package.AppImage \
  --repo pencil2d/pencil \
  --signer-workflow pencil2d/pencil/.github/workflows/ci.yml \
  --source-ref refs/tags/vX.Y.Z \
  --deny-self-hosted-runners
```

For an official nightly, use `--source-ref refs/heads/master` and also pass
`--source-digest FULL_COMMIT_SHA` for the expected commit from that build's
GitHub run. Binding the expected commit prevents accepting a different valid
build. Use a published `refs/heads/release/...` ref when verifying a release
branch build.

For a third-party fork, use that fork's repository and signer-workflow identity,
together with its expected ref and commit. A fork attestation identifies the
fork; it does not establish endorsement by Pencil2D.

For verification using a downloaded bundle, add:

```sh
--bundle ./pencil2d-provenance.sigstore.json
```

Keep the repository, workflow, ref, and commit checks when using a bundle.
Online verification also checks the current repository identity; bundle-only
verification cannot detect later repository/owner renames or name reuse.
Fully offline use additionally needs a trusted root prepared according to the
[GitHub CLI verification documentation](https://cli.github.com/manual/gh_attestation_verify).

Attestations authenticate the bytes and their GitHub Actions origin. They do
not establish that the source or dependencies are defect-free, or that builds
are reproducible. Existing platform code signing and notarization remain
additional checks.

## Publishing releases

Copy the exact final packages, matching `.sha256` files, and provenance bundle
from the same successful trusted CI run to the GitHub release/download page.
Do not rebuild, recompress, or sign those packages after hashing/attestation.
If a package changes, publish a new checksum and a new authenticated build;
the old attestation cannot authenticate changed bytes.

The workflow does not create releases or modify the separate download service.
Maintainers publishing through those surfaces must expose the verification
files as well. Manual signing of Windows releases after CI changes the bytes
and requires a fresh checksum; CI provenance for the earlier unsigned package
does not verify that manually modified package.

The provenance job receives OIDC and attestation permissions only for trusted
push/manual runs on the listed refs. Build and pull request jobs retain
read-only permissions. See GitHub's
[artifact attestation guide](https://docs.github.com/en/actions/how-tos/secure-your-work/use-artifact-attestations/use-artifact-attestations).

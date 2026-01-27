# Auto-detect Release vs Development Builds from Version Number

## Problem

Currently, both CMake and qmake require manual flags (`-DPENCIL2D_RELEASE=ON`, `PENCIL2D_RELEASE=1`) to distinguish release builds from development/nightly builds. This has led to real-world packaging issues:

- **Fedora** previously had Pencil2D incorrectly marked as a dev build
- **Arch Linux** still has this issue
- Packagers must remember to pass these flags or the release will be marked incorrectly

Our current approach is not the industry conventions where software automatically detects its release status based on the version being built.

## Feedback

This issue was raised from a PR review:
https://github.com/pencil2d/pencil/pull/1958#discussion_r2633460171

> "Honestly we really ought to take care of this stuff by default. If you build from a release tag or tarball, you shouldn't have to pass extra options like this. [...] it's the general convention that if you obtain sources for a particular release 1.2.3, the software will "know" that it is that particular release and will be marked accordingly even without doing anything special.
>
> Most software works like this. By going against this widespread convention in Pencil2D, we're defying the expectations of people building the software, and that's exactly how we end up with a release version of Pencil2D being incorrectly marked as a dev build in Arch/Fedora."

## Solution

Use the **`VERSION` variable** in qmake to automatically distinguish between release and development builds.

### How It Works

1. **Default VERSION**: If not specified, defaults to `0.0.0.0` (set in `util/common.pri`)
2. **GitHub Actions**: Sets `VERSION_NUMBER` environment variable based on branch/tag, which is passed to qmake
3. **Build Detection**: `util/common.pri` checks the `VERSION` value to determine build type

### Version Patterns by Source

| Source | VERSION_NUMBER | Build Type | Example |
|--------|---------------|------------|---------|
| **Tag build** (e.g., `v0.7.1`) | `v0.7.1` | Release | Tag name with 'v' prefix |
| **Release branch** (e.g., `release/0.7.1`) | `0.7.1-b12345` | Release | Version + build number |
| **Master branch** | `99.0.0.12345` | Nightly | Special version pattern |
| **Feature/other branches** | Not set (defaults to `0.0.0.0`) | Development | Local development |
| **Local build** | `0.0.0.0` | Development | No VERSION specified |

### Build Type Detection Logic

The build system (`util/common.pri`) checks the `VERSION` variable:

- **Development/Local builds**:
  - `VERSION` equals `0.0.0.0` (the default value set in `util/common.pri`)
  - Used for feature branches and local development

- **Nightly builds**:
  - `VERSION` matches `99.0.0.*` pattern
  - Used for master branch CI builds
  
- **Release builds**:
  - Any other version (e.g., `0.7.1`, `v0.7.1`, `0.7.1-b12345`, manually specified versions)
  - Automatically enables `QT_NO_DEBUG_OUTPUT`

## Build Commands

```bash
# Development build (default, VERSION=0.0.0.0)
qmake
# Output: App Version: 0.0.0.0
#         Build Type: Development

# Nightly build (from master branch on Github Actions)
qmake "VERSION=99.0.0.12345"
# Output: App Version: 99.0.0.12345
#         Build Type: Nightly

# Release build (from tag)
qmake # the VERSION is set manually in the common.pri

# Outpuutil/common.pri` - Add VERSION default and auto-detection logic
- [x] `.github/actions/setup-environment/setup-versions.sh` - Set VERSION_NUMBER based on git ref
- [x] `.github/actions/create-package/action.yml` - Detect release builds from tags and release branches
- [x] `.github/workflows/ci.yml` - Pass VERSION to qmake
- [ ] Documentation (update build instructions if needed)

## Backward Compatibility

The manual `PENCIL2D_NIGHTLY` and `PENCIL2D_RELEASE` flags are no longer needed and can be removed since the build type is now auto-detected from the VERSION

## CI/CD Workflow

### Version Assignment in GitHub Actions

1. **Tag builds** (`v0.7.1`): `VERSION_NUMBER=v0.7.1`
2. **Release branches** (`release/0.7.1`): `VERSION_NUMBER=0.7.1-b${BUILD_NUMBER}`
3. **Master branch**: `VERSION_NUMBER=99.0.0.${BUILD_NUMBER}`
4. **Other branches**: No VERSION_NUMBER set, falls back to `0.0.0.0`

### Package Naming

- **Tag builds**: `pencil2d-mac-arm64-v0.7.1.zip`
- **Release branches**: `pencil2d-mac-arm64-0.7.1-b12345.zip`
- **Master/dev builds**: `pencil2d-mac-arm64-b12345-20260129.zip`

## Files to Modify

- [x] `pencil2d.pro` - Add VERSION = 99.99.0
- [x] `util/common.pri` - Add BUILD_NUMBER support and auto-detection
- [ ] CI configuration files (if they currently pass manual flags)
- [ ] Documentation (build instructions)

## Backward Compatibility

The manual `PENCIL2D_NIGHTLY` and `PENCIL2D_RELEASE` options should be removed entirely since they'll no longer be needed.

## Related

- Issue #1948 (CMake build system)
- PR #1958

#!/usr/bin/env python3
"""Create/check portable SHA-256 sidecars for final Pencil2D packages.

Checksums detect changed bytes. Authenticate downloads separately with the
GitHub build attestation (see docs/download-verification.md).
"""
# SPDX-License-Identifier: GPL-2.0-only
import argparse
import hashlib
import re
import sys
from pathlib import Path


def safe_name(name):
    if not name or name in (".", "..") or any(c in name for c in "/\\\r\n\0"):
        raise ValueError("Unsafe package filename")
    return name


def digest(path):
    if path.is_symlink() or not path.is_file():
        raise ValueError(f"Not a regular package file: {path}")
    result = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            result.update(block)
    return result.hexdigest()


def create(path):
    name = safe_name(path.name)
    checksum = digest(path)
    sidecar = path.with_name(name + ".sha256")
    if sidecar.is_symlink():
        raise ValueError(f"Refusing to overwrite a symbolic link: {sidecar}")
    # Binary mode prevents Windows newline translation. Compatible with sha256sum.
    sidecar.write_bytes(f"{checksum} *{name}\n".encode("utf-8"))
    return sidecar


def verify(sidecar):
    if sidecar.is_symlink():
        raise ValueError(f"Not a regular checksum file: {sidecar}")
    with sidecar.open("rb") as stream:
        data = stream.read(4097)
    if len(data) > 4096:
        raise ValueError("Checksum file is too large")
    match = re.fullmatch(r"([0-9a-f]{64}) [ *]([^\r\n]+)\n?", data.decode("utf-8"))
    if not match:
        raise ValueError("Expected exactly one SHA-256 checksum record")
    checksum, name = match.groups()
    name = safe_name(name)
    if sidecar.name != name + ".sha256":
        raise ValueError("Checksum filename does not match the package filename")
    package = sidecar.with_name(name)
    if digest(package) != checksum:
        raise ValueError(f"SHA-256 mismatch: {package}")
    return package


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("create", "verify"))
    parser.add_argument("files", nargs="+", type=Path)
    args = parser.parse_args(argv)
    try:
        for path in args.files:
            if args.command == "create":
                print(create(path))
            else:
                print(f"OK: {verify(path)}")
    except (OSError, ValueError, UnicodeError) as error:
        print(f"Verification failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

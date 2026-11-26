# SPDX-License-Identifier: GPL-2.0-only
import importlib.util
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "download_verification.py"
spec = importlib.util.spec_from_file_location("download_verification", SCRIPT)
verification = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verification)


class DownloadVerificationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.package = self.root / "pencil2d mac é.zip"
        self.package.write_bytes(b"final signed package\0\xff")
        self.sidecar = verification.create(self.package)

    def test_round_trip_and_deterministic_standard_format(self):
        self.assertEqual(verification.verify(self.sidecar), self.package)
        original = self.sidecar.read_bytes()
        verification.create(self.package)
        self.assertEqual(self.sidecar.read_bytes(), original)
        self.assertRegex(original, rb"^[0-9a-f]{64} \*pencil2d mac .*\.zip\n$")
        self.assertNotIn(b"\r", original)

    def test_tampered_package_and_checksum_fail(self):
        original = self.package.read_bytes()
        self.package.write_bytes(original + b"tampered")
        with self.assertRaisesRegex(ValueError, "mismatch"):
            verification.verify(self.sidecar)
        self.package.write_bytes(original)
        self.sidecar.write_bytes(b"0" * 64 + b" *" + self.package.name.encode() + b"\n")
        with self.assertRaisesRegex(ValueError, "mismatch"):
            verification.verify(self.sidecar)

    def test_missing_file_fails(self):
        self.package.unlink()
        with self.assertRaises(ValueError):
            verification.verify(self.sidecar)

    def test_multiple_records_and_bad_encodings_fail(self):
        record = self.sidecar.read_bytes()
        for data in (record + record, b"invalid", b"\xff", b"x" * 4097):
            with self.subTest(data=data[:30]):
                self.sidecar.write_bytes(data)
                with self.assertRaises((ValueError, UnicodeError)):
                    verification.verify(self.sidecar)

    def test_paths_and_ambiguous_filenames_are_rejected(self):
        for name in ("../outside", "/outside", "C:\\outside", ".", "..", "a\nb", "a\rb", "a\0b"):
            with self.subTest(name=name):
                with self.assertRaises(ValueError):
                    verification.safe_name(name)
        original = self.sidecar.read_bytes()
        renamed = self.root / "different.sha256"
        renamed.write_bytes(original)
        with self.assertRaisesRegex(ValueError, "filename"):
            verification.verify(renamed)

    def test_symlinks_are_rejected(self):
        link = self.root / "link.zip"
        try:
            link.symlink_to(self.package)
        except OSError:
            self.skipTest("Creating symlinks is unavailable")
        with self.assertRaises(ValueError):
            verification.create(link)
        self.sidecar.unlink()
        self.sidecar.symlink_to(self.package)
        with self.assertRaises(ValueError):
            verification.create(self.package)

    def test_empty_package_can_be_verified(self):
        self.package.write_bytes(b"")
        verification.create(self.package)
        self.assertEqual(verification.verify(self.sidecar), self.package)

    def test_cli_nonzero_on_tampering_and_missing_inputs(self):
        result = subprocess.run([sys.executable, str(SCRIPT), "verify", str(self.sidecar)], capture_output=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.package.write_bytes(b"corrupt")
        result = subprocess.run([sys.executable, str(SCRIPT), "verify", str(self.sidecar)], capture_output=True)
        self.assertNotEqual(result.returncode, 0)
        result = subprocess.run([sys.executable, str(SCRIPT), "verify"], capture_output=True)
        self.assertNotEqual(result.returncode, 0)


if __name__ == "__main__":
    unittest.main()

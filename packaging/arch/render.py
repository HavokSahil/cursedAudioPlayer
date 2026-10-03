#!/usr/bin/env python3
"""Render a checksummed AUR recipe from the exact release source archive."""
import hashlib
import pathlib
import re
import sys

archive = pathlib.Path(sys.argv[1])
output = pathlib.Path(sys.argv[2])
match = re.fullmatch(r'cursedap-(\d+\.\d+\.\d+)\.tar\.gz', archive.name)
if not match:
    raise SystemExit('Expected cursedap-X.Y.Z.tar.gz')
version = match[1]
digest = hashlib.sha256(archive.read_bytes()).hexdigest()
text = pathlib.Path(__file__).with_name('PKGBUILD.in').read_text()
text = text.replace('@VERSION@', version).replace('@SOURCE_SHA256@', digest)
output.mkdir(parents=True, exist_ok=True)
(output / 'PKGBUILD').write_text(text)

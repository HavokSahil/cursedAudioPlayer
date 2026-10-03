# Linux releases

The executable and package name are `cursedap`. Release 1.1.0 contains native
x86_64 packages for Arch, Debian 12, Ubuntu 24.04, Fedora 43, and openSUSE Leap 16.
Choose the asset built for your distribution; RPM packages are built separately
for Fedora and openSUSE. These are downloadable packages, not apt/yum repositories.

Download packages and `SHA256SUMS` from the matching GitHub release, then verify:

```sh
sha256sum --ignore-missing -c SHA256SUMS
```

Install the matching asset:

```sh
sudo pacman -U ./cursedap-*.pkg.tar.zst
sudo apt install ./cursedap*debian12.deb          # Debian 12
sudo apt install ./cursedap*ubuntu24.04.deb      # Ubuntu 24.04
sudo dnf install ./cursedap*fedora43.rpm         # Fedora 43
sudo zypper install ./cursedap*opensuse-leap16.rpm
cursedap ~/Music/song.flac
man cursedap
```

## Release workflow

`.github/workflows/release.yml` builds source and distro packages, runs tests in
each environment, and publishes a release only after all package jobs pass.
It uses the built-in `GITHUB_TOKEN`; no separate release account or token is needed.
GitHub Actions must be enabled in the repository's Settings → Actions → General.
For each release, update the version in `CMakeLists.txt` and the manual page, commit,
then push the matching tag:

```sh
git tag -a v1.1.0 -m 'cursedap 1.1.0'
git push origin v1.1.0
```

Manual reruns must select the version tag, not the default branch. Release packages
include all bundled library notices. To reproduce DEB or RPM packages, install the
build dependencies listed in the workflow and run `packaging/build-linux.sh DEB
/tmp/packages` or `packaging/build-linux.sh RPM /tmp/packages` from the source root.
The source archive bundles ECQT and pinned PFFFT; no submodule download is required.

## Arch AUR / yay

The release's `cursedap-aur.tar.gz` contains a checksummed `PKGBUILD` and `.SRCINFO`.
It is built and tested by `makepkg` in Arch before publication. Until an AUR entry
exists, use the direct Arch package with `pacman -U`; `yay -S cursedap` requires
an actual AUR submission.

When registration is available, register at https://aur.archlinux.org/register,
confirm the email, and add an SSH public key in the account settings. An existing
AUR maintainer can also submit the recipe while new registration is unavailable.
Check whether the package name already exists before creating a new entry.
For an available name, use the account's SSH identity:

```sh
git clone ssh://aur@aur.archlinux.org/cursedap.git
# Extract the release recipe elsewhere, then copy PKGBUILD and .SRCINFO here.
cd cursedap
makepkg --verifysource
makepkg --printsrcinfo > .SRCINFO
git add PKGBUILD .SRCINFO
git commit -m 'Release cursedap 1.1.0'
git push
```

After AUR publication, users can install with `yay -S cursedap`. Future releases
must update both recipe files with the exact source-archive SHA-256.
Generate them using `python3 packaging/arch/render.py cursedap-X.Y.Z.tar.gz /tmp/aur`
and `makepkg --printsrcinfo` in that directory. The stable recipe never uses `SKIP`.

Official references: [GitHub releases and Actions](https://docs.github.com/en/actions),
[PKGBUILD](https://man.archlinux.org/man/PKGBUILD.5.en),
[CPack DEB](https://cmake.org/cmake/help/latest/cpack_gen/deb.html), and
[CPack RPM](https://cmake.org/cmake/help/latest/cpack_gen/rpm.html).

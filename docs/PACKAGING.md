# Packaging And Release Delivery

The `0.1.0` candidate supports installation as a CMake package. Release delivery
is configured in `.github/workflows/ci.yml`; a configured workflow is not evidence
that a public release has been published. See `RELEASE_CHECKLIST.md` for tag gates
and `PROJECT_STATUS.md` for executed checks.

## Build And Install From Source

```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cmake --build build/release --config Release --parallel
ctest --test-dir build/release -C Release --output-on-failure
cmake --install build/release --config Release --prefix /absolute/path/to/cnumkit
```

Use `BUILD_SHARED_LIBS=ON` for a shared library. Multi-configuration generators,
such as Visual Studio, select the build type with `--config`; use the same
configuration for build, test, install, and consumer commands.

An installation contains public headers under `include/`, libraries under
`lib/` (or the platform's CMake install libdir), exported package metadata under
`lib/cmake/cnumkit/`, and offline documentation and the MIT license under
`share/doc/cnumkit/`. Windows shared builds also install a DLL under `bin/`.
The generated export header is installed in place of the source fallback.

## Consume A Binary Archive

The tag workflow prepares these assets:

| Archive | Build | Intended toolchain |
| --- | --- | --- |
| `cnumkit-linux-shared.tar.gz` | Debug shared | GCC on the workflow's Ubuntu runner |
| `cnumkit-linux-static.tar.gz` | Release static | GCC on the workflow's Ubuntu runner |
| `cnumkit-windows-shared.zip` | Debug shared | MSVC on the workflow's Windows runner |
| `cnumkit-windows-static.zip` | Release static | MSVC on the workflow's Windows runner |

These are native binaries, not universal packages. Match the operating system,
architecture, compiler/runtime, and build configuration. In particular, MSVC
Debug artifacts require the matching debug runtime and are intended for
development. Use a source build for another toolchain, architecture, or runtime.
GitHub's source archives contain source code rather than installed packages.

Download `SHA256SUMS` alongside the four archives. On Linux, verify downloaded
assets with `sha256sum --ignore-missing -c SHA256SUMS`. On PowerShell, compare
`Get-FileHash -Algorithm SHA256 <archive>` with its entry in `SHA256SUMS`.

Extract the chosen archive to a directory of your choice. For example:

```bash
mkdir -p /absolute/path/to/cnumkit
tar -xzf cnumkit-linux-static.tar.gz -C /absolute/path/to/cnumkit
cmake -S tests/consumer -B build/consumer \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/absolute/path/to/cnumkit
cmake --build build/consumer --config Release --parallel
ctest --test-dir build/consumer -C Release --output-on-failure
```

In your own project, use:

```cmake
find_package(cnumkit 0.1 CONFIG REQUIRED)
target_link_libraries(your_application PRIVATE cnumkit::cnumkit)
```

The imported target carries include paths, library locations, and required math
linkage. Keep the installation tree together when moving it. For Windows shared
packages, add `<prefix>/bin` to the process `PATH` before running an application:

```powershell
$env:PATH = 'C:/path/to/cnumkit/bin;' + $env:PATH
ctest --test-dir build/consumer -C Debug --output-on-failure
```

The package version file accepts compatible `0.1.x` requests. This is a pre-1.0
API policy, not a promise of binary compatibility between different toolchains.

## Maintainer Verification And Publication

For each non-sanitized, non-contract CI configuration, the workflow installs
the library, archives the install tree, moves the original prefix aside, and
extracts the archive into a new prefix. A separate consumer then finds, links,
and runs against that extracted package. This verifies archive contents and
relocatability as well as basic installed API use. The normal test matrix also
enables the C++ public-header smoke test explicitly.

Before tagging, finish the candidate checks in `RELEASE_CHECKLIST.md`, record
the candidate commit and CI links, and date the changelog. The version in
`CMakeLists.txt`, `Doxyfile`, release notes, and the `vX.Y.Z` tag must agree.
Creating or pushing a tag is a separate maintainer action.

On a version tag, publication waits for documentation, Linux, and Windows jobs.
The release job rejects a tag that differs from the CMake version, requires all
four binary archives, generates and verifies `SHA256SUMS`, then publishes those
five assets using `RELEASE_NOTES.md`. GitHub supplies source archives separately.
To audit a published release, retain the tag commit, workflow run URL, compiler
versions from the configure logs, and asset checksums with the release evidence.

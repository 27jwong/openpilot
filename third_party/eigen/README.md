# Eigen

Header-only linear algebra library, vendored as a fallback for hosts with no other copy.

- Version: 3.4.0
- Source: <https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz>
- sha256: `8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72`
- Contents: the tarball's `Eigen/` tree, unmodified, at `include/eigen3/Eigen`, plus its
  `COPYING.*` files. `unsupported/` and the tests/benchmarks/docs are not vendored.

## Why

`common/transformations`, `rednose`, and the generated locationd models include
`<eigen3/Eigen/Dense>`. AGNOS used to install `libeigen3-dev` into the rootfs, but
[agnos-builder `2d374185a`](https://github.com/commaai/agnos-builder/commit/2d374185a)
replaced it with the `eigen` package in the managed venv, so on-device builds failed
with `fatal error: 'eigen3/Eigen/Dense' file not found`.

`SConstruct` prefers that managed package (`eigen.INCLUDE_DIR`, same as `capnproto`
and `ffmpeg`) and falls back to this copy. Both are passed as `-isystem` rather than
added to `CPPPATH`: the build runs `-Werror -Wshadow`, and Eigen's headers only pass
because a system include dir silences their warnings. `-I` dirs are searched first,
so macOS builds still pick up brew's eigen3 from `$(brew --prefix)/include`.

## Updating

Download a release tarball, verify its checksum, and replace `include/eigen3/Eigen`
with the tarball's `Eigen/` directory. Keep the license files in sync.

## License

MPL2 (`COPYING.MPL2`); see `COPYING.README` for the per-file details. The LGPL-licensed
parts of Eigen live in `unsupported/`, which is not vendored here.

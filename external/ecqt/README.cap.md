# Bundled ECQT

Source: https://github.com/havoksahil/ecqt
Revision: 85b1010bf400c32f846f49d780d51a820d2b2877
License: MIT; see LICENSE and source headers.

PFFFT source: https://bitbucket.org/jpommier/pffft
Pinned submodule revision: d7a4c0206a29423478776d6b23a37bbb308f21d5
PFFFT's license is retained at the top of pffft/pffft.c and pffft/pffft.h.

The three ECQT headers and two PFFFT source files are bundled so ordinary builds
work offline and do not fetch changing dependencies.

Local integration fixes:

- Align variable-length note windows to the newest audio in the rolling stream.
- Grow sparse buffers during initialization; transforms still allocate no memory.
- Preserve all N frequency bins when sparsifying each Nk-sample temporal kernel.
- Free FFT setup and aligned allocations on success and all failure paths.
- Reject invalid configurations and propagate kernel-capacity failures.
- Allow larger FFT dimensions instead of silently clipping them to 32767 rows.
- Avoid integer overflow in sparse allocation sizes; fix vec_init_mem's unconditional return.
- Use the window bank's actual WTYPE macro and rename cabs to avoid the C complex builtin.
- Make nextPow2 handle the native unsigned-long width and overflow.
- Reject null transform contexts and update result status.

features/CQTBackend.c confines the C implementation to one translation unit.
features/CQT.h provides the C++ RAII wrapper and rolling analysis window.

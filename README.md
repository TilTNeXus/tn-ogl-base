# TiltNexus' OpenGL project base

A fairly simple (for better or for worse) base for my OpenGL projects. It is made with SDL3, OpenGL, OpenAL, stb-image, and stb-vorbis. It can load these file types: OBJs, OGGs, TTFs, most images, and GLSL shaders.

Most parts of this are not expandable in any way because I don't look into the future at all.

## Build Instructions
### Linux:
* Install SDL3, FreeType, GLEW, and OpenAL-Soft with your package manager
* run `make`

### Mac:
* Install [brew](https://brew.sh/)
* Install SDL3, FreeType, GLEW, and OpenAL-Soft with brew
* run `make mac`

## Windows:
* Install [MSYS2](https://www.msys2.org/)
* Install SDL3, FreeType, GLEW, and OpenAL-Soft with MSYS2
* run `make win`


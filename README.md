# BootanimationBot

Bot implementation of Bootanimation, tool used to convert bootanimation.zip (and optional bootaudio.mp3) into a single video (bootanimation.mp4).

# Usage examples

# Before starting usage, you must create a token.h with your own bot token in the MyBot folder:
```c
#define TOKEN "<your token>"
```

# Currently only conversion to MP4 is supported.

## 1. Just animation
```
/tomp4 anim:<uploaded bootanimation.zip file>
```

## 2. Animation + audio
```
/tomp4 anim:<uploaded bootanimation.zip file> audio:<uploaded bootaudio.mp3 file>
```

## 3. Animation + dynamic coloring (end colors)
NOTE: Color input is foramtted as <R>,<G>,<B>,<A>
```
/tomp4 anim:<uploaded bootanimation.zip file> audio:<uploaded bootaudio.mp3 file> dyncolor:<dynamic color in the foramt given>
``` 
# Dependencies and base

The project is based on the D++ bot template, licensed under Apache-2.0. The project also depends on a copy of ImageMagick for building. The variant used is 8-bit quantum.

For building notes read [this document](PrerequiresiteSetup.md)

# What it can't do at the moment

1. Fade effect in animation (don't have an example to implement)

2. Clock and progress in bootanimation (and probably will never be done)

3. Play till completion

4. Samsung/Quram QMG decode support (will implement later under Dynarmic emulation)

5. Pantech SKY LZ frames format (will implement later)

# License(s)

At this moment the project does not have a license. I planned to license it under Apache-2.0 but x264 and FDK-AAC licenses have incompatibilities with Apache-2.0.

Its dependencies are under the following licenses:

1. D++ - [Apache-2.0](LICENSE.DPP)

2. ImageMagick - [ImageMagick license](LICENSE.IMAGEMAGICK)

3. VideoLAN X264 (modified for Visual Studio building) - [GPL 2.0 or later](LICENSE.X264)

4. FDK-AAC - [Fraunhofer license](LICENSE.FDKAAC)

5. libzip (modified for Visual Studio building) - [BSD 3-clause](LICENSE.LIBZIP)

6. miniaudio - [MIT No Attribution](LICENSE.MINIAUDIO)

7. OGG and Vorbis - [BSD 3-Clause](LICENSE.OGGVORBIS)

8. zlib (modified for Visual Studio building) - [MIT License](LICENSE.ZLIBWIN), [ZLIB license](LICENSE.ZLIB)

9. L-SMASH (modified to build under VIsual Studio 2022) - [ISC](LICENSE.LSMASH)
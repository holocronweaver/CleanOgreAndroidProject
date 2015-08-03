# Clean OGRE Project for Android

A basic NDK Android application powered by the OGRE 3D rendering
engine. There is practically no Java layer - the main() function is in
C++. Assuming it works on your device, it should display a black ogre head silhouette on a solid blue background:

![Black ogre head silhoutte on blue background](https://github.com/holocronweaver/CleanOgreAndroidProject/blob/demo/screenshots/ogre-android-bare.png)

Has been tested on Android 4 and 5 for `armeabi` and `armeabi-v7a` using
OGRE 1.9 and 1.10 for the GLES2 render system.

Current implementation is based closely on the OGRE 1.10 SampleBrowser
for Android. It has RTSS support for fixed-function emulation and
fallback shaders. OIS input is partially implemented.

To build on Linux, run `build.sh`. Similar scripts can be easily made from this script for other OS.

To debug using gdb, run `adk-gdb --start`.

You should alter variables in Android.mk to reflect your project.

Released under the MIT license.

# TODO

* Clean up code and remove unnecessary or dead sections.
* Integrate this work into the BareOgre repo.

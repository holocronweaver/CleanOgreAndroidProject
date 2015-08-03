#! /usr/bin/env sh
ndk-build
if [ ! -e obj/ ]; then
    # Should only be required to initialize project.
    android update project --name OgreJNI --path . \
            --target "android-19" --subprojects
fi
ant clean
ant debug
adb install -r bin/OgreNativeAndroidApplication-debug.apk

LOCAL_PATH := $(call my-dir)

# Plugin_OctreeSceneManagerStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := Plugin_OctreeSceneManagerStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libPlugin_OctreeSceneManagerStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# Plugin_ParticleFXStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := Plugin_ParticleFXStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libPlugin_ParticleFXStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreVolumeStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreVolumeStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgreVolumeStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgrePagingStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgrePagingStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgrePagingStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreOverlayStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreOverlayStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgreOverlayStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreMeshLodGeneratorStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreMeshLodGeneratorStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgreMeshLodGeneratorStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreTerrainStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreTerrainStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgreTerrainStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreRTShaderSystemStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreRTShaderSystemStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgreRTShaderSystemStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# RenderSystem_GLES2Static
	include $(CLEAR_VARS)
	LOCAL_MODULE    := RenderSystem_GLES2Static
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libRenderSystem_GLES2Static.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreMainStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreMainStatic
	LOCAL_SRC_FILES := /home/jesse/lib/build/android-ogre/lib/libOgreMainStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# FreeImage
	include $(CLEAR_VARS)
	LOCAL_MODULE    := FreeImage
	LOCAL_SRC_FILES := /media/Soliton/lib/android-ndk-r10e/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/user/lib/libFreeImage.a
	include $(PREBUILT_STATIC_LIBRARY)

# OIS
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OIS
	LOCAL_SRC_FILES := /media/Soliton/lib/android-ndk-r10e/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/user/lib/libOIS.a
	include $(PREBUILT_STATIC_LIBRARY)

# freetype
	include $(CLEAR_VARS)
	LOCAL_MODULE    := freetype
	LOCAL_SRC_FILES := /media/Soliton/lib/android-ndk-r10e/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/user/lib/libfreetype.a
	include $(PREBUILT_STATIC_LIBRARY)

# zziplib
	include $(CLEAR_VARS)
	LOCAL_MODULE    := zziplib
	LOCAL_SRC_FILES := /media/Soliton/lib/android-ndk-r10e/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/user/lib/libzziplib.a
	include $(PREBUILT_STATIC_LIBRARY)

    include $(CLEAR_VARS)
    LOCAL_MODULE    := OgreNativeAndroidApplication
    LOCAL_LDLIBS	:= -landroid -lc -lm -ldl -llog -lEGL -lGLESv2
    LOCAL_LDLIBS	+= -L/home/jesse/lib/build/android-ogre/lib -L/home/jesse/lib/ogredeps/lib/armeabi


    LOCAL_STATIC_LIBRARIES	+=  Plugin_OctreeSceneManagerStatic Plugin_ParticleFXStatic OgreVolumeStatic OgrePagingStatic OgreOverlayStatic OgreMeshLodGeneratorStatic OgreRTShaderSystemStatic OgreTerrainStatic RenderSystem_GLES2Static OgreMainStatic
    LOCAL_STATIC_LIBRARIES	+=  FreeImage
    LOCAL_STATIC_LIBRARIES	+=  OIS freetype zziplib
    LOCAL_STATIC_LIBRARIES	+=  android_native_app_glue cpufeatures
    LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES=1
    LOCAL_CFLAGS += -fexceptions -frtti -x c++ -D___ANDROID___ -DANDROID -DZZIP_OMIT_CONFIG_H -DUSE_RTSHADER_SYSTEM=1

    # LOCAL_LDLIBS	+= -lPlugin_ParticleFXStatic -lPlugin_OctreeSceneManagerStatic -lRenderSystem_GLES2Static -lOgreRTShaderSystemStatic -lOgreOverlayStatic -lOgreTerrainStatic -lOgrePagingStatic -lOgreVolumeStatic -lOgreMainStatic
    # LOCAL_LDLIBS	+= -lzzip -lz -lFreeImage -lfreetype -lOIS \
    # 					   @OGRE_ROOT/@ANDROID_BUILD/systemlibs/@ARMEABI/libsupc++.a \
    # 					   @OGRE_ROOT/@ANDROID_BUILD/systemlibs/@ARMEABI/libstdc++.a \
    # 					   @OGRE_ROOT/@ANDROID_BUILD/SampleBrowserNDK/obj/local/@ARMEABI/libcpufeatures.a

	LOCAL_C_INCLUDES := /home/jesse/lib/build/android-ogre/include /home/jesse/lib/ogre/OgreMain/include /home/jesse/lib/ogre/RenderSystems/GLES2/include /home/jesse/lib/ogre/RenderSystems/GLES2/include/EGL
	LOCAL_C_INCLUDES += /media/Soliton/lib/android-ndk-r10e/sources/cpufeatures /home/jesse/lib/ogre/Components/RTShaderSystem/include /home/jesse/lib/ogre/Components/MeshLodGenerator/include /home/jesse/lib/ogre/Components/Overlay/include /home/jesse/lib/ogre/Components/Volume/include /home/jesse/lib/ogre/Components/Terrain/include /home/jesse/lib/ogre/Components/Paging/include
	LOCAL_C_INCLUDES += /home/jesse/lib/ogre/PlugIns/ParticleFX/include /home/jesse/lib/ogre/PlugIns/OctreeSceneManager/include 
	LOCAL_C_INCLUDES += /home/jesse/lib/ogredeps/include /home/jesse/lib/ogredeps/include/OIS 

        LOCAL_PATH := /home/jesse/lib/build/android-ogre/CleanOgreAndroidProject/jni/
        LOCAL_SRC_FILES := AndroidNative.cpp

    include $(BUILD_SHARED_LIBRARY)

    $(call import-module,android/cpufeatures)
    $(call import-module,android/native_app_glue)

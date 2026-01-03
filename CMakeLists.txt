cmake_minimum_required(VERSION 3.23.1)

project(AnalyzerHost VERSION 0.0.1 LANGUAGES C CXX)

# 1. Global configuration and JUCE
# ------------------------------------------------------------------------------
set(PROJECT_NAME "AnalyzerHost")
set(PRODUCT_NAME "AnalyzerHost")
set(COMPANY_NAME "AlbertPhaseLab")
set(BUNDLE_ID "com.albertphaselab.AnalyzerHost")

# We load JUCE before defining the plugin
add_subdirectory(modules/JUCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "" FORCE) 
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)
endif()

# 2. Define the Plugin
# ------------------------------------------------------------------------------
juce_add_plugin("${PROJECT_NAME}"
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"
    PLUGIN_MANUFACTURER_CODE "APlb"
    PLUGIN_CODE "AnHt"
    FORMATS VST3 Standalone AU
    PRODUCT_NAME "${PRODUCT_NAME}"
    NEEDS_MIDI_INPUT TRUE         # Enabled for communication with internal plugins
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    IS_SYNTH FALSE
    AU_MAIN_TYPE kAudioUnitType_MusicEffect
    COPY_PLUGIN_AFTER_BUILD TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
)

# 3. Source files
# ------------------------------------------------------------------------------
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"
)
target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

# 4. Settings and Definitions (To avoid Headless mode)
# ------------------------------------------------------------------------------
target_compile_definitions("${PROJECT_NAME}" PRIVATE
    JUCE_WEB_BROWSER=0 
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    # We forced JUCE to know that a screen was available
    JUCE_DISPLAY_NONE=0
    # We ensure that the audio formats are compiled with GUI support.
    JUCE_UNIT_TESTS=0
)

target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source")

# 5. Link to Libraries
# ------------------------------------------------------------------------------
# Note: Explicitly included juce_audio_formats for addDefaultFormats()
target_link_libraries("${PROJECT_NAME}" PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_audio_devices
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_graphics
    juce::juce_dsp
    juce::juce_data_structures
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
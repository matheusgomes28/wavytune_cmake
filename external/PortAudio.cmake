include(FetchContent)

FetchContent_Declare(
    portaudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio
    GIT_TAG v19.7.0
)

FetchContent_MakeAvailable(portaudio)

set_target_properties(portaudio PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(portaudio_static PROPERTIES LINKER_LANGUAGE CXX)
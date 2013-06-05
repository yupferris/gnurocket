#gcc example_sdl.c OpenGLUtils.c -lSDL -lSDL_mixer -lGL -lGLU ./libsync.so && LD_PRELOAD=./libsync.so ./a.out


find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_library(M_LIBRARY m)
add_executable(sdl example_sdl.c)
target_link_libraries(sdl sync ${M_LIBRARY} ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${OPENGL_LIBRARIES})

file(COPY tune.ogg DESTINATION ../)

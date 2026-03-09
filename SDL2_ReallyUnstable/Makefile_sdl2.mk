# the root of the SDL2 directory on your project
SDL2_ROOT_DIR ?= libs/sdl_n64/SDL2
# bpp to create the display with. DEPTH_16_BPP or DEPTH_32_BPP
SDL2_BPP ?= DEPTH_16_BPP

N64_CFLAGS   += -D_POSIX_C_SOURCE=0 -D__MIPS__ -DN64_BPP=$(SDL2_BPP) -I$(SDL2_ROOT_DIR)/include
N64_CXXFLAGS += -I$(SDL2_ROOT_DIR)/include

# warnings from original code
N64_CFLAGS += -Wno-error=maybe-uninitialized -Wno-error=use-after-free

C_SDL_FILES  := $(wildcard ${SDL2_ROOT_DIR}/src/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/atomic/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/audio/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/cpuinfo/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/dynapi/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/events/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/file/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/haptic/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/hidapi/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/libm/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/locale/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/misc/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/power/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/render/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/render/*/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/sensor/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/stdlib/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/thread/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/timer/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/video/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/video/yuv2rgb/*.c)

C_SDL_FILES  += $(wildcard ${SDL2_ROOT_DIR}/src/sensor/dummy/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/haptic/dummy/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/locale/dummy/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/misc/dummy/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/thread/generic/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/joystick/*.c)

C_SDL_FILES  += $(wildcard ${SDL2_ROOT_DIR}/src/timer/n64/*.c) \
                                $(wildcard ${SDL2_ROOT_DIR}/src/audio/n64/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/joystick/n64/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/video/n64/*.c) \
				$(wildcard ${SDL2_ROOT_DIR}/src/render/n64/*.c)
CC = gcc -pipe -c -O3 -march=native -Wall -Iinclude
LD = -mwindows

LINUX_XCB_CC = $(CC) -DSR_PLATFORM_LINUX -DSR_WINDOW_USE_XCB
LINUX_XCB_LD = -lxcb -lxcb-image

DEBUG_CC = $(CC) -DSR_DEBUG
LINUX_XCB_DEBUG_CC = $(LINUX_XCB_CC) -DSR_DEBUG

all:
	@ $(CC) source/core.c -o build/core.o
	@ $(CC) source/math.c -o build/math.o
	@ $(CC) source/thread.c -o build/thread.o
	@ $(CC) source/window.c -o build/window.o
	@ $(CC) source/scene.c -o build/scene.o
	@ $(CC) source/object.c -o build/object.o
	@ $(CC) source/renderer.c -o build/renderer.o
	@ $(CC) source/softrays.c -o build/softrays.o

	@ gcc build/* -o SoftRays $(LD)
	@ del /q build\*

debug:
	@ $(DEBUG_CC) source/core.c -o build/core.o
	@ $(DEBUG_CC) source/math.c -o build/math.o
	@ $(DEBUG_CC) source/thread.c -o build/thread.o
	@ $(DEBUG_CC) source/window.c -o build/window.o
	@ $(DEBUG_CC) source/scene.c -o build/scene.o
	@ $(DEBUG_CC) source/object.c -o build/object.o
	@ $(DEBUG_CC) source/renderer.c -o build/renderer.o
	@ $(DEBUG_CC) source/softrays.c -o build/softrays.o

	@ gcc build/* -o SoftRays $(LD)
	@ del /q build\*

linux_xcb:
	@ mkdir -p build

	@ $(LINUX_XCB_CC) source/core.c -o build/core.o
	@ $(LINUX_XCB_CC) source/math.c -o build/math.o
	@ $(LINUX_XCB_CC) source/thread.c -o build/thread.o
	@ $(LINUX_XCB_CC) source/window.c -o build/window.o
	@ $(LINUX_XCB_CC) source/scene.c -o build/scene.o
	@ $(LINUX_XCB_CC) source/object.c -o build/object.o
	@ $(LINUX_XCB_CC) source/renderer.c -o build/renderer.o
	@ $(LINUX_XCB_CC) source/softrays.c -o build/softrays.o

	@ gcc build/* -o SoftRays $(LINUX_XCB_LD)
	@ rm -r build

linux_xcb_debug:
	@ mkdir -p build

	@ $(LINUX_XCB_DEBUG_CC) source/core.c -o build/core.o
	@ $(LINUX_XCB_DEBUG_CC) source/math.c -o build/math.o
	@ $(LINUX_XCB_DEBUG_CC) source/thread.c -o build/thread.o
	@ $(LINUX_XCB_DEBUG_CC) source/window.c -o build/window.o
	@ $(LINUX_XCB_DEBUG_CC) source/scene.c -o build/scene.o
	@ $(LINUX_XCB_DEBUG_CC) source/object.c -o build/object.o
	@ $(LINUX_XCB_DEBUG_CC) source/renderer.c -o build/renderer.o
	@ $(LINUX_XCB_DEBUG_CC) source/softrays.c -o build/softrays.o

	@ gcc build/* -o SoftRays $(LINUX_XCB_LD)
	@ rm -r build

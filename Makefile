LINUX_XCB_CC = -DSR_PLATFORM_LINUX -DSR_WINDOW_USE_XCB
LINUX_XCB_LD = -lxcb -lxcb-image

LINUX_XCB_DEBUG_CC = $(LINUX_XCB_CC) -DSR_DEBUG
LINUX_XCB_DEBUG_LD = $(LINUX_XCB_LD)

CC = x86_64-linux-gnu-gcc -pipe -c -O3 -march=native -Wall -Iinclude

all:
	@ mkdir -p build

	@ $(CC) source/core.c -o build/core.o
	@ $(CC) source/math.c -o build/math.o
	@ $(CC) source/thread.c -o build/thread.o
	@ $(CC) source/window.c -o build/window.o
	@ $(CC) source/scene.c -o build/scene.o
	@ $(CC) source/object.c -o build/object.o
	@ $(CC) source/renderer.c -o build/renderer.o
	@ $(CC) source/softrays.c -o build/softrays.o

	@ x86_64-linux-gnu-gcc build/* -o SoftRays $(LD)
	@ rm -r build

linux_xcb:
	@ mkdir -p build

	@ $(CC) $(LINUX_XCB_CC) source/core.c -o build/core.o
	@ $(CC) $(LINUX_XCB_CC) source/math.c -o build/math.o
	@ $(CC) $(LINUX_XCB_CC) source/thread.c -o build/thread.o
	@ $(CC) $(LINUX_XCB_CC) source/window.c -o build/window.o
	@ $(CC) $(LINUX_XCB_CC) source/scene.c -o build/scene.o
	@ $(CC) $(LINUX_XCB_CC) source/object.c -o build/object.o
	@ $(CC) $(LINUX_XCB_CC) source/renderer.c -o build/renderer.o
	@ $(CC) $(LINUX_XCB_CC) source/softrays.c -o build/softrays.o

	@ x86_64-linux-gnu-gcc build/* -o SoftRays $(LINUX_XCB_LD)
	@ rm -r build

linux_xcb_debug:
	@ mkdir -p build

	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/core.c -o build/core.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/math.c -o build/math.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/thread.c -o build/thread.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/window.c -o build/window.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/scene.c -o build/scene.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/object.c -o build/object.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/renderer.c -o build/renderer.o
	@ $(CC) $(LINUX_XCB_DEBUG_CC) source/softrays.c -o build/softrays.o

	@ x86_64-linux-gnu-gcc build/* -o SoftRays $(LINUX_XCB_DEBUG_LD)
	@ rm -r build

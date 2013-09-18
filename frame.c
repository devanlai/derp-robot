
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>

#include "frame.h"

typedef struct {
	int fd;
	ChumbyPixel* ptr;
} _Framebuffer;

static _Framebuffer _fb_map[CHUMBY_NUM_FB] = {
	(_Framebuffer){.fd=-1, .ptr=NULL},
	(_Framebuffer){.fd=-1, .ptr=NULL}
};

ChumbyPixel rgb_to_pixel(uint8_t red, uint8_t green, uint8_t blue) {
	return ((red&0xf8)<<8) + ((green&0xfc)<<3) + ((blue&0xf8)>>3);
}

ChumbyPixel* map_framebuffer(FBNum fb_no) {
	if (fb_no < 0 || fb_no >= CHUMBY_NUM_FB) {
		return NULL;
	}

	int fd = _fb_map[fb_no].fd;
	if (fd < 0) {
		return NULL;
	}

	if (_fb_map[fb_no].ptr == NULL) {
		ChumbyPixel* data = mmap(NULL, CHUMBY_FB_SIZE, PROT_WRITE, MAP_SHARED | MAP_NORESERVE, fd, 0);
		if (data == MAP_FAILED) {
			return NULL;
		}
		_fb_map[fb_no].ptr = data;
	}
	return _fb_map[fb_no].ptr;
}

void unmap_framebuffer(ChumbyPixel* buffer) {
	if (buffer == NULL) {
		return;
	}
	munmap(buffer, CHUMBY_FB_SIZE);
}

int open_framebuffer(FBNum fb_no) {
	if (fb_no < 0 || fb_no >= CHUMBY_NUM_FB) {
		return -1;
	}

	int fd = _fb_map[fb_no].fd;
	if (fd < 0) {
		switch (fb_no) {
		case FB0:
			fd = open("/dev/fb0", O_RDWR);
			break;
		case FB1:
			fd = open("/dev/fb1", O_RDWR);
			break;
		default:
			return -1;
		}
		if (fd < 0) {
			return fd;
		} else {
			_fb_map[fb_no].fd = fd;
			return fd;
		}
	} else {
		return fd;
	}
}

void close_framebuffer(FBNum fb_no) {
	if (fb_no < 0 || fb_no >= CHUMBY_NUM_FB) {
		return;
	}

	int fd = _fb_map[fb_no].fd;
	if (fd >= 0) {
		close(fd);
		_fb_map[fb_no].fd = -1;
	}
}

ChumbyPixel* init_framebuffer(FBNum fb_no) {
	int fd = open_framebuffer(fb_no);
	if (fd < 0) {
		return NULL;
	}
	return map_framebuffer(fb_no);
}

void free_framebuffer(FBNum fb_no) {
	if (fb_no < 0 || fb_no >= CHUMBY_NUM_FB) {
		return;
	}
	unmap_framebuffer(_fb_map[fb_no].ptr);
	close_framebuffer(fb_no);
}

void fill(ChumbyPixel* buf, ChumbyPixel color) {
	int i;
	for (i=0; i < CHUMBY_WIDTH*CHUMBY_HEIGHT; i++) {
		buf[i] = color;
	}
}

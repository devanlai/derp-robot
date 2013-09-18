#ifndef _FRAME_H_
#define _FRAME_H_

#ifdef FALCONWING
#define CHUMBY_WIDTH 320
#define CHUMBY_HEIGHT 240
#else
#define CHUMBY_WIDTH 800
#define CHUMBY_HEIGHT 600
#endif

#define CHUMBY_FB_SIZE (CHUMBY_WIDTH*CHUMBY_HEIGHT*sizeof(ChumbyPixel))

#ifdef FALCONWING
#define CHUMBY_RED_DEPTH 5
#define CHUMBY_GREEN_DEPTH 6
#define CHUMBY_BLUE_DEPTH 5
#define CHUMBY_ALPHA_DEPTH 0
#else
#define CHUMBY_RED_DEPTH 8
#define CHUMBY_GREEN_DEPTH 8
#define CHUMBY_BLUE_DEPTH 8
#define CHUMBY_ALPHA_DEPTH 8
#endif

#define CHUMBY_RED_MAX   ((1 << (CHUMBY_RED_DEPTH)) - 1)
#define CHUMBY_GREEN_MAX ((1 << (CHUMBY_GREEN_DEPTH)) - 1)
#define CHUMBY_BLUE_MAX  ((1 << (CHUMBY_BLUE_DEPTH)) - 1)
#define CHUMBY_ALPHA_MAX ((1 << (CHUMBY_ALPHA_DEPTH)) - 1)


#define CHUMBY_NUM_FB 2

#define PIXEL_INDEX(X,Y) (CHUMBY_WIDTH*(Y)+(X))

#ifdef FALCONWING
typedef uint16_t ChumbyPixel;
#else
typedef uint32_t ChumbyPixel;
#endif

typedef enum {FB0=0, FB1=1} FBNum;

ChumbyPixel rgb_to_pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

int open_framebuffer(FBNum fb_no);
void close_framebuffer(FBNum fb_no);

ChumbyPixel* map_framebuffer(FBNum fb_no);
void unmap_framebuffer(ChumbyPixel* buffer);

ChumbyPixel* init_framebuffer(FBNum fb_no);
void free_framebuffer(FBNum fb_no);

void fill(ChumbyPixel* buf, ChumbyPixel color);

#endif

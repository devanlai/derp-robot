#ifndef _FRAME_H_
#define _FRAME_H_

#define CHUMBY_WIDTH 320
#define CHUMBY_HEIGHT 240
#define CHUMBY_FB_SIZE (CHUMBY_WIDTH*CHUMBY_HEIGHT*sizeof(ChumbyPixel))

#define CHUMBY_RED_DEPTH 5
#define CHUMBY_GREEN_DEPTH 6
#define CHUMBY_BLUE_DEPTH 5

#define CHUMBY_NUM_FB 2

#define PIXEL_INDEX(X,Y) (CHUMBY_WIDTH*(Y)+(X))

typedef uint16_t ChumbyPixel;
typedef enum {FB0=0, FB1=1} FBNum;

ChumbyPixel rgb_to_pixel(uint8_t red, uint8_t green, uint8_t blue);

int open_framebuffer(FBNum fb_no);
void close_framebuffer(FBNum fb_no);

ChumbyPixel* map_framebuffer(FBNum fb_no);
void unmap_framebuffer(ChumbyPixel* buffer);

ChumbyPixel* init_framebuffer(FBNum fb_no);
void free_framebuffer(FBNum fb_no);

void fill(ChumbyPixel* buf, ChumbyPixel color);

#endif

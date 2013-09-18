#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <rfb/rfb.h>
#include <rfb/rfbclient.h>

#include "robot.h"
#include "frame.h"
//#include "ChumbyTouchFrame/screen.h"
//#include "ChumbyTouchFrame/touch.h"

static ChumbyPixel* fb = NULL, *fb0 = NULL, *fb1 = NULL;

static int is_big_endian(void)
{
    union {
            uint32_t i;
            char c[4];
          }
    bint = {0x01020304};
                        
    return bint.c[0] == 1; 
}

static void updateChumbyFrameBuffer(rfbClient* client, int x, int y, int w, int h) {
	if (x >= CHUMBY_WIDTH || y >= CHUMBY_HEIGHT) return;
	if (x+w >= CHUMBY_WIDTH)
		w = CHUMBY_WIDTH - 1 - x;
	if (y+h >= CHUMBY_HEIGHT)
		h = CHUMBY_HEIGHT - 1 - y;

	uint8_t bytes_per_pixel = client->format.bitsPerPixel / 8;
	uint32_t i,j;

	for (j=y; j < y+h; j++) {
		uint32_t row_offset = bytes_per_pixel * j * client->width;
		for (i=x; i < x+w; i++) {
			uint32_t col_offset = bytes_per_pixel * i;
			uint32_t remote_pixel = *((uint32_t*)(client->frameBuffer + row_offset + col_offset));
			if (client->si.format.bigEndian) {
				if (client->format.bitsPerPixel == 16) {
					remote_pixel = rfbClientSwap16IfLE(remote_pixel);
				} else if (client->format.bitsPerPixel == 32) {
					remote_pixel = rfbClientSwap32IfLE(remote_pixel);
				}
			}
			uint32_t r,g,b;
			//Extract the individual r/g/b values from the host-side
			r = (remote_pixel >> client->format.redShift) & client->format.redMax;
			g = (remote_pixel >> client->format.greenShift) & client->format.greenMax;
			b = (remote_pixel >> client->format.blueShift) & client->format.blueMax;

			//Rescale to the local r/g/b maximums
			r = (r * CHUMBY_RED_MAX) / client->format.redMax;
			g = (g * CHUMBY_GREEN_MAX) / client->format.greenMax;
			b = (b * CHUMBY_BLUE_MAX) / client->format.blueMax;
			fb[PIXEL_INDEX(i,j)] = rgb_to_pixel(r, g, b, 255);
		}
	}


	printf("trueColour: %u\r\n", client->format.trueColour);
	printf("Shifts: %d, %d, %d\r\n", client->format.redShift, client->format.greenShift, client->format.blueShift);
	printf("Maxes: %d, %d, %d\r\n",
	       client->format.redMax,
	       client->format.greenMax,
	       client->format.blueMax);

}

static void refreshChumbyFrameBuffer(rfbClient* client) {
	printf("Refreshing\r\n");
}

static void setServerHost(rfbClient *client, char *hostname) {
	if (client->serverHost) {
		free(client->serverHost);
	}

	client->serverHost = strdup(hostname);
}

static void setServerPort(rfbClient *client, int port) {
	client->serverPort = port;
}

int main(int argc, char *argv[]) {
	printf("Hello, world!\r\n");

	if (argc < 2 || argc > 3) {
		printf("Usage: %s address [port]\r\n", argv[0]);
		return 0;
	}


	rfbClient *client = NULL;

	//Create an RFB client with default settings, 16-bit color
#ifdef FALCONWING
	client = rfbGetClient(5, 3, 2);
#else
	client = rfbGetClient(8, 3, 4);
#endif
	//Configure host information
	setServerHost(client, argv[1]);

	int port = 5900;
	if (argc == 3) {
		port = atoi(argv[2]);
	}

	setServerPort(client, port);

	//Configure callbacks
	client->GotFrameBufferUpdate = updateChumbyFrameBuffer;
	client->FinishedFrameBufferUpdate = refreshChumbyFrameBuffer;

	//Configure client capabilities
	client->canHandleNewFBSize = FALSE;
	client->appData.viewOnly = TRUE;


	printf("Connecting to %s:%d\r\n", argv[1], port);

	rfbBool success = ConnectToRFBServer(client, client->serverHost, client->serverPort);

	if (!success) {
		fprintf(stderr, "Couldn't establish TCP connection to %s:%d\r\n",
				client->serverHost, client->serverPort);
		if (client)
			rfbClientCleanup(client);
		return 1;
	}

	success = InitialiseRFBConnection(client);

	if (!success) {
		fprintf(stderr, "Couldn't initialize RFB connection\r\n");
		if (client)
			rfbClientCleanup(client);
		return 2;
	}

	success = SetFormatAndEncodings(client);

	if (!success) {
		fprintf(stderr, "Couldn't set format/encoding parameters\r\n");
		if (client)
			rfbClientCleanup(client);
		return 3;
	}

	client->width = client->si.framebufferWidth;
	client->height = client->si.framebufferHeight;
	success = client->MallocFrameBuffer(client);

	if (!success) {
		fprintf(stderr, "Couldn't allocate space for local framebuffer\r\n");
		if (client)
			rfbClientCleanup(client);
		return 4;
	}

	//success = SendScaleSetting(client, 1);

	printf("VNC setup done\r\n");

	printf("Initializing screen\r\n");
	fb0 = init_framebuffer(FB0);
	if (!fb0) {
		fprintf(stderr, "Couldn't open screen fb0\r\n");
		if (client)
			rfbClientCleanup(client);
		return 5;
	}
	/*
	fb1 = init_framebuffer(FB1);
	if (!fb1) {
		fprintf(stderr, "Couldn't open screen fb1\r\n");
		if (client)
			rfbClientCleanup(client);
		return 6;
	}
	*/
	fill(fb0, rgb_to_pixel(0,0,0,255));
	//fill(fb1, rgb_to_pixel(0,0,0,255));

	fb = fb0;

	printf("Processing RFB messages\r\n");
	//Process RFB messages
	unsigned int count = 0;
	while (1) {
		int status = WaitForMessage(client, 500);
		count++;
		if (status < 0) {
			printf("Duh-oh\r\n");
			break;
		} else if (status > 0) {
			status = HandleRFBServerMessage(client);
			if (!status)
				break;
		}
		if (count >= 2000) {
			count = 0;
			SendFramebufferUpdateRequest(client, 0, 0, CHUMBY_WIDTH, CHUMBY_HEIGHT, FALSE);
		}
	}

	if (client)
		rfbClientCleanup(client);

	if (fb0) {
		free_framebuffer(FB0);
	}

	if (fb1) {
		free_framebuffer(FB1);
	}

    return 0;
}

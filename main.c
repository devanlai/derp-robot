#include <stdlib.h>
#include <stdio.h>
#include <rfb/rfbclient.h>

#include "robot.h"
#include "ChumbyTouchFrame/screen.h"
#include "ChumbyTouchFrame/touch.h"

static void updateChumbyFrameBuffer(rfbClient* client, int x, int y, int w, int h) {
	if (x >= CHUMBY_WIDTH || y >= CHUMBY_HEIGHT) return;
	if (x+w >= CHUMBY_WIDTH)
		w = CHUMBY_WIDTH - 1 - x;
	if (y+h >= CHUMBY_HEIGHT)
		h = CHUMBY_HEIGHT - 1 - y;


	int i;
	for (i=y; i < y+h; i++) {
		memcpy(&framebuffer[CHUMBY_ROW_STRIDE*i], client->frameBuffer, CHUMBY_PX_SIZE*w);
	}
}

static void refreshChumbyFrameBuffer(rfbClient* client) {
	printf("Refreshing\r\n");
	set_screen();
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
	client = rfbGetClient(5, 3, 2);

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
    int okay = init_screen();
	if (!okay) {
		fprintf(stderr, "Couldn't open screen\r\n");
		if (client)
			rfbClientCleanup(client);
		return 5;
	}

	char BLACK[2] = {0,0};
	set_colour(BLACK);
	set_screen();
	

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
			SendFramebufferUpdateRequest(client, 0, 0, 320, 240, FALSE);
		}
	}

	if (client)
		rfbClientCleanup(client);

    return 0;
}

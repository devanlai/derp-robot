#include <stdlib.h>
#include <stdio.h>
#include <rfb/rfbclient.h>

#include "robot.h"

static rfbBool testResize(rfbClient *client) {
	printf("Resize Request:\r\n");
	printf("\twidth:%d\r\n\theight:%d\r\n\tdepth:%d\r\n",
		   client->width, client->height, client->format.bitsPerPixel);

	//Don't actually do anything yet.
	client->width = 0;
	client->height = 0;

	return FALSE;
}

int main(int argc, char *argv[]) {
	printf("Hello, world!\r\n");

	rfbClient *client = NULL;

	//The chumby one has a 16-bit RGB touchscreen
	client = rfbGetClient(5, 3, 2);

	//Configure callbacks
	client->MallocFrameBuffer = testResize;

	rfbBool success = rfbInitClient(client, &argc, argv);
	if (!success) {
		fprintf(stderr, "Couldn't initialize client\r\n");
		client = NULL; //Freed by rfbInitClient on failure
		return 1;
	}

	printf("Processing RFB messages\r\n");
	//Process RFB messages
	while (1) {
		int status = WaitForMessage(client, 5000);
		if (status < 0) {
			break;
		} else if (status > 0) {
			printf("Got some sort of RFB message\r\n");
			status = HandleRFBServerMessage(client);
			if (!status)
				break;
		}
	}

	if (client)
		rfbClientCleanup(client);

    return 0;
}

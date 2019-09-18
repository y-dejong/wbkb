#include "wiiuse.h"

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

#include <xdo.h>

#define MAX_WIIMOTES 4

#define MIN_WEIGHT 25
#define X_THRESHOLD 0.3f
#define Y_THRESHOLD 0.3f

const char * LEFT_KEY;
const char * RIGHT_KEY;
const char * UP_KEY;
const char * DOWN_KEY;

xdo_t *xdo;
int max_x = 0, max_y = 0, min_x = 0, min_y = 0;
bool running = true;
int x_dir = 0, y_dir = 0; //-1 down/left, 1 up/right

const char* left_key, right_key, up_key, down_key;

void handle_event(struct wiimote_t* wm) {
	//printf("\n\n---EVENT [id %i] ---\n", wm->unid);
	if (wm->exp.type == EXP_WII_BOARD) {
		/* wii balance board */
		struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
		float total = wb->tl + wb->tr + wb->bl + wb->br;
		float x = ((wb->tr + wb->br) / total) * 2 - 1;
		float y = ((wb->tl + wb->tr) / total) * 2 - 1;
		if(total > MIN_WEIGHT) {
			if(x > X_THRESHOLD && x_dir != 1) {
				x_dir = 1;
				printf("Right!\n");
				xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, RIGHT_KEY, 0);
			}
			if(x < X_THRESHOLD * -1 && x_dir != -1) {
				x_dir = -1;
				printf("Left!\n");
				xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, LEFT_KEY, 0);
			}
			if(x > X_THRESHOLD * -1 && x < X_THRESHOLD) {
				if(x_dir == -1) {
                    xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, LEFT_KEY, 0);
					x_dir = 0;
                    printf("Released left!\n");
                }
				if(x_dir == 1) {
                    xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, RIGHT_KEY, 0);
					x_dir = 0;
                    printf("Released right\n");
                }
			}
			if(y > Y_THRESHOLD && y_dir != 1) {
				y_dir = 1;
				printf("Up!\n");
				xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, UP_KEY, 0);
			}
			if(y < Y_THRESHOLD * -1 && y_dir != -1) {
				y_dir = -1;
				printf("Down!\n");
				xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, DOWN_KEY, 0);
			}
			if(y > Y_THRESHOLD * -1 && y < Y_THRESHOLD) {
				if(y_dir == 1) {
                    xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, UP_KEY, 0);
                    y_dir = 0;
                    printf("Released up!\n");
                }
				if(y_dir == -1) {
                    xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, DOWN_KEY, 0);
                    y_dir = 0;
                    printf("Released down!\n");
                }
			}
		} else {
			if(x_dir == 1) {
                xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, RIGHT_KEY, 0);
				x_dir = 0;
                printf("Released right! (Low weight)\n");
            }
			if(x_dir == -1) {
                xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, LEFT_KEY, 0);
				x_dir = 0;
                printf("Released left! (Low weight)\n");
            }
			if(y_dir == -1) {
                xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, DOWN_KEY, 0);
				y_dir = 0;
                printf("Released down! (Low weight)\n");
            }
			if(y_dir == 1) {
                xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, UP_KEY, 0);
				y_dir = 0;
                printf("Released up! (Low weight)\n");
            }
		}
	}
}

short any_wiimote_connected(wiimote** wm, int wiimotes) {
	int i;
	if (!wm) {
		return 0;
	}

	for (i = 0; i < wiimotes; i++) {
		if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
			return 1;
		}
	}

	return 0;
}

void help() {
	printf("USAGE: wiikey [--help] [LEFT_KEY RIGHT_KEY UP_KEY DOWN_KEY]\n");
	printf("Map directional movement on the Wii Balance Board to keyboard presses\n");
	printf("\nOPTIONS:\n");
	printf("\t-h, --help\tDisplay this text and exit\n");
	printf("\t[DIRECTION]_KEY\tThe button to be held when leaning in DIRECTION\n");
	printf("\nPairing Instructions:\n");
	printf("\tLocate the \"Reset\" button on the bottom of the Wii Balance Board.\n");
	printf("\tThis may be located inside the battery compartment. Turn on\n");
	printf("\tBluetooth on your device. Run the program, and press the Reset\n");
	printf("\tbutton when given the \"Ready to connect\" message.\n");
}

void quit(int i) {
	running = false;
	printf("Quitting!\n");
}

int main(int argc, char** argv) {

    LEFT_KEY = "Left", RIGHT_KEY = "Right", UP_KEY = "Up", DOWN_KEY = "Down";
	if(argc > 1) {
		for(int i = 1; i < argc; ++i) {
			if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
				help();
				return 0;
			}
		}
		if(argc >= 5) LEFT_KEY = argv[1], RIGHT_KEY = argv[2], UP_KEY = argv[3], DOWN_KEY = argv[4];
	}

	printf("LEFT is %s, RIGHT is %s, UP is %s, DOWN is %s\n", LEFT_KEY, RIGHT_KEY, UP_KEY, DOWN_KEY);
	signal(SIGINT, quit);
	
	wiimote** wiimotes = wiiuse_init(MAX_WIIMOTES);
	int found, connected;

	// Find wiimotes. Last argument is seconds until timeout
printf("Ready to connect to Wii Balance Board\n");
	found = wiiuse_find(wiimotes, MAX_WIIMOTES, 10);
	if (!found) {
		printf("No wiimotes found.\n");
		return 0;
	}

	/*
	 *	Connect to the wiimotes
	 *
	 *	Now that we found some wiimotes, connect to them.
	 *	Give the function the wiimote array and the number
	 *	of wiimote devices we found.
	 *
	 *	This will return the number of established connections to the found wiimotes.
	 */
	connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
	if (connected) {
		printf("Connected to %i wiimotes (of %i found).\n", connected, found);
	} else {
		printf("Failed to connect to any wiimote.\n");
		return 0;
	}
	usleep(200000);

	xdo = xdo_new(":0.0");
	
	printf("\nConnected, ready to interact!\n");
	while (running && any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {
		if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
			/*
			 *	This happens if something happened on any wiimote.
			 *	So go through each one and check if anything happened.
			 */
			int i = 0;
			for (; i < MAX_WIIMOTES; ++i) {
				switch (wiimotes[i]->event) {
				case WIIUSE_EVENT:
						/* a generic event occurred */
						handle_event(wiimotes[i]);
						break;

				case WIIUSE_DISCONNECT:
				case WIIUSE_UNEXPECTED_DISCONNECT:
					/* the wiimote disconnected */
					printf("\n--- DISCONNECTED [wiimote id %i] ---\n", wiimotes[i]->unid);
					break;

				case WIIUSE_WII_BOARD_CTRL_REMOVED:
					/* some expansion was removed */
					printf("An expansion was removed.\n");
					break;
					
				default:
					break;
				}
			}
		}
	}

	/*
	 *	Disconnect the wiimotes
	 */
	wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
	xdo_free(xdo);
	printf("End of main\n");

	return 0;
}

# WBKB: Wii Board Keyboard

--Work in Progress--

Official Git: https://github.com/y-dejong/wbkb

## About

Wii Board Keyboard lets you easily map movements on a Wii Balance Board to your keyboard. Currently supports leaning forward, backwards, left, and right.

Distributed under the GNU GPLv3.

## Dependencies

Only runs on Linux, although a Windows version is a high priority. (There are currently no plans for a Mac build).
Required packages:
* `libxdo3` and `libxdo-dev` to simulate keypresses
* `bluez` drivers to connect to the balance board
* BlueZ dev files (`libbluetooth-dev` package for Debian/Ubuntu)

## Compiling

You need CMake installed in order to compile.

	mkdir build
	cd build
	cmake ..
	make

## Running

The wbkb program should be run in a shell.

### Keybindings

Running the `wbkb` program on its own will, by default, bind each direction on the balance board to its corresponding arrow key. You can change which keys are bound by passing them in as command line arguments, in the form:
	./wbkb LEFT_KEY RIGHT_KEY UP_KEY DOWN_KEY

### Pairing

To pair the board, start by locating the "Reset" button on the bottom of the Wii Balance Board. It may be located inside the battery compartment. This puts the board in discoverable mode for bluetooth. Turn on Bluetooth on your device. Run the program, and press the Reset button when given the `Ready to connect` prompt. You do *not* have to select the board from your device's bluetooth menu, and the indicator light on the balance board may continue blinking while you use the program. This is normal.

### Playing

Once you are given the `Ready to Interact` prompt, the balance board will begin to send keypresses based on what direction you are leaning. Note that the board will not send keypresses unless there are 25 kg on the board, and there is a threshold that you must lean past in order to send a keypress (Both of these values will be configurable in future versions).

You can quit the program with `Ctrl-C`.
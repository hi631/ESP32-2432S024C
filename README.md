Sample of ESP32-2432S024C running on arduino
arduinoで動作するESP32-2432S024Cのサンプル
日本語解説は[Ｑｉｉｔａ](https://qiita.com/hi631/items/2fdc19d1e440f98330b1)を参照。

Operation screen
  ![Image01.jpg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/159764/f8ac881f-f4a4-f34f-7bb1-54f8b4d5e2c9.jpeg)

A simple explanation of the operation of the reference program is shown below.

1.LED
To confirm the operation of the board, the 3-color LED flashes.

2.graphicstest
For LCD confirmation, I assigned the 24C pin to Adafruit's ILI9341 sample.
*The reason why it works slowly is probably because it is general purpose.

3.touch1
Operation confirmation of CST820 used for touch panel. Added display when touching two points to bb_captouch.
Touch information is output serially, so be sure to display the serial monitor.

4.touch2
Same as above, the touch results are displayed on the LCD.
*Touch two points to clear the screen. *I feel like the second point is in the wrong position.

5.Keypad_240x320
Enter numbers using Touch Pal.
For checking the cooperative operation of Touch Pal and LCD input and display areas.
*Since the calibration has not been taken, if the position is misaligned, it will be necessary to correct it using the program.

6.MovingIcons
Example of operation using LovyanGFX. I just put the pins together.
If you have temporarily updated the library, it should work just by defining ESP32-2432S024C.
　*I was a little surprised at how fast the drawing was with this well-made library.

7. Ball Maze
It seems to be a game where you rotate the screen left and right to drop the ball in the maze and escape from the maze.
Similarly, this demo software has been modified to work with ESP32-2432S024C.
Since the 24C does not have physical buttons, the button functions were built into the panel.
To rotate, touch the left or right near the center of the screen to start rotating to the right or left. *These are the original specifications.
The scrolling part at the top is the magnification ratio adjustment, which can be changed by sliding left and right.
The thing that is loose at the bottom is the rotation speed adjustment, which can be changed by sliding left and right.
*The flickering is not the desired action, but is due to poor timing of drawing, but I can't understand it well.

8.Factory_samples_Capacitive_touch
Sample software that was originally written.
To compile this program, the following changes are required.
Copy User_Setup.h and lv_conf.h to the specified location in the TFT_eSPI library.
Copy the library's lvgl\src\demos folder directly under the lvgl folder.

# EggLight

Chickens need light to produce consistently through the winter.

This code is being used on a Arduino uno in my chicken house.

You may need to set the time on your rtc.

The hours for the light is set in the code. Default is on @ 6:00, off at 20:00 (I think). If the time is between start and stop, the light comes on if the light is below the threshold point. The threshold is set by pushing the switch.

Made with PlatformIO which is better than Arduino IDE (because I can use Emacs ;) )

If you are new to platformio, or interested in using my code here, please contact me. (alanwedel19@gmail.com)
I would be happy to explain every thing about this simple timer.

libnrf24
========

A library for communicate with nRF24L01 chips using user space SPI and GPIO.

Written in C for easy integration into luajit.

Developed for RaspberryPi.

Hardware requirements
---------------------

 * A nRF24L01 chip connected to a SPI bus and a GPIO pin (CE) available in user space (/dev/spidev, /sys/class/gpio).

Copyright
---------

Released under the MIT license.

(C) 2013 Erik Svensson, <erik.public@gmail.com>.

---
title: "underwater rover"
date: 2014-01-01
videos: ["103242633"]
---
*summer 2014*

This is an underwater rover that was developed at Lockheed Martin as a prototype for the [Engineering Explorers Post program](http://www.ourshadesofblue.org/documents/lockheedmartinengineeringexplorerspost.pdf).

The rover is propelled by 3 bilge pumps, 2 on the sides for forward jetting and 1 mounted underneath with a propellor for up and down movement. An Arduino Pro Mini receives serial commands through an XBee radio which floats on top of the water in a tethered buoy (because wireless signals cannot be transmitted through water). The side bilge pump motors are controlled through 2 MOSFETs while the motor underneath is controlled through a L298 H-bridge. Speed is regulated with PWM signals from the Arduino.

On the front of the rover is a clear acrylic dome and a small camera which streams wireless video to the rover operator. The wireless video transmitter is also housed in the buoy to allow for proper transmission. The camera is mounted on a servo which allows the operator to tilt the view up and down using a joystick on the controller. Data from sensors mounted in and around the rover are overlayed with the video feed using a [video experimenter shield](http://nootropicdesign.com/ve/). There are waterproof LEDs mounted to the body of the rover in order to see underwater.

The rover is powered by a 3 cell LiPo battery which is also housed in the buoy. Power lines are run through the tether along with the communication line (Ethernet cable).

Check out the rover schematic and Arduino code below!

This project was funded by the [New York NASA Space Grant Consortium](http://astrosun2.astro.cornell.edu/specialprograms/spacegrant/). Created with Matt Churchman and Matthew Liu.

<!-- Project Images -->
{{< figure src="images/1.jpg" alt="1" >}}
{{< figure src="images/2.JPG" alt="2" >}}
{{< figure src="images/3.rovercircuitboard.JPG" alt="3 rovercircuitboard" >}}
{{< figure src="images/4.JPG" alt="4" >}}
{{< figure src="images/5.JPG" alt="5" >}}
{{< figure src="images/6.JPG" alt="6" >}}

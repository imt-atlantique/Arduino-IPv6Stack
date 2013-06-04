Arduino µIPv6 Stack
=================

An IPv6 stack for [Arduinos](http://www.arduino.cc/) MEGA and [Xbee](http://www.digi.com/en/products/wireless/point-multipoint/xbee-series1-module) 
based on [Contiki OS](http://www.contiki-os.org/) network stack. µIPv6 has been created because
we think Arduino is a good prototyping platform and because Arduino has to be IPv6 ready.

If you are looking for **Arduino UNO** IPv6 stack, please have a look at our [Arduino pico IPv6](https://github.com/telecombretagne/Arduino-pIPv6Stack) project !

![Arduino µIPv6 Stack](http://departements.telecom-bretagne.eu/data/rsm/stack%20diagram.png)


Features
--------


* IP network stack with low-power standards : [6LoWPAN](http://datatracker.ietf.org/wg/6lowpan/charter/), [RPL](http://tools.ietf.org/html/rfc6550), and [CoAP](http://datatracker.ietf.org/doc/draft-ietf-core-coap/)
* Compatible with [Arduino MEGA](http://arduino.cc/en/Main/ArduinoBoardMega2560) family
* Based on [Contiki OS](http://www.contiki-os.org/) network stack
* Can be used on top of [IEEE 802.15.4](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-series1-module), IEEE 802.3 (Ethernet), IEEE 802.11 (Wifi) MAC layers *

*: We are only providing [XBee Series 1](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-series1-module) interface for now.

Get Started
-----------

To get started using µIPv6 stack, go to the [wiki](https://github.com/telecombretagne/Arduino-IPv6Stack/wiki). The wiki
contains a step-by-step tutorial and examples necessary to build your first Arduino Wireless Sensor Network.

![Arduino WSN](http://departements.telecom-bretagne.eu/data/rsm/heterogeneous%20wsn%20arch.png)


License
-------
 
This stack is open source software : it can be freely used both in commercial and non-commercial systems and the full source code is available.


People
-------

[Alejandro Lampropulos](alejandro.lampropulos@telecom-bretagne.eu) (Télecom Bretagne, RSM dpt) : *project maintainer*

[Baptiste Gaultier](baptiste.gaultier@telecom-bretagne.eu) (Télecom Bretagne, RSM dpt) : *developer and documentation maintainer*

[Laurent Toutain](Laurent.Toutain@telecom-bretagne.eu) (Télecom Bretagne, RSM dpt) : *associate professor and project lead*

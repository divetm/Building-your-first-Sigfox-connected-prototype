# Building your first Sigfox connected prototype
*Learn how to build your own Sigfox-connected device with a devkit and a couple of components*

Introduction
----
The goal of this repo is to learn how to build your own IoT device prototype using a Sigfox-connected devkit and a couple of components.  
If you've read [this repo](https://github.com/divetm/Getting-started-with-Sigfox) before, some of this might feel like a déjà-vu. The idea is always to guide you from zero to a hundred. But, if you don't want to re-read something you already know, you can jump to [this section](#building-your-own-prototype).  

1. [A quick overview of what Sigfox is](#a-quick-overview-of-what-sigfox-is)  
  a. [The technology](#the-technology)  
  b. [Its limits and its advantages](#its-limits-and-its-advantages)  
  c. [The value chain](#the-value-chain)  

2. [Getting started](#getting-started)  
	a. [Devkits and firware](#devkits-and-firmware)  
	b. [Sigfox Backend, Sigfox Cloud Platform, REST API and Callbacks](#sigfox-backend-sigfox-cloud-platform-rest-api-and-callbacks)    
	c. [Using a platform to design your Data workflow](#using-a-platform-to-design-your-data-workflow)  
	d. [Making your device request a downlink](#making-your-device-request-a-downlink)

3. [Building your own prototype](#building-your-own-prototype)  
  a. [Methodology](#methodology)  
  b. [The hardware](#the-hardware)  
  c. [The firmware](#the-firmware)  
  d. [Using Losant to design your data workflow](#using-losant-to-design-your-data-workflow)

A quick overview of what Sigfox is
----
### The Technology
Sigfox is a Low Power Wide Area Network (LPWAN) complementing existing networks to address the bulk of connected objects.  
Sigfox invented a Radio Protocol and used it to build a global network adapted to small data transactions (like the ones generated by objects). Sigfox does not build IoT solutions or sell chipsets. Other IoT networks, like LoRa, exist.


<p align="center">
  <img width="900" src="https://github.com/divetm/Getting-started-with-Sigfox/blob/master/images/sigfox_network.png">
</p>


Sigfox's network enables your everyday object to send data captured by its sensors.  
The data can then be processed and give new insights on optimizable processes.  

### Its limits and its advantages
It is important to understand that the payload (data) that can be sent over Sigfox's network (uplink message) is limited in:  
* size (maximum 12 bytes) - this is intrinsic to how the LPWAN technology works, we will come back to that;
* frequency (140 messages a day max) - that is due to the fact that Sigfox uses a public radio frequency and cannot hog it

However, the size limit is actually not that penalizing. Take a look at what can be transmitted with as few as 12 bytes:


<p align="center">
  <img width="900" src="what can be sent with 12 bytes">
</p>


And with 140 uplink messages a day, you could be sending a message every ~10 minutes!

On the other hand, Sigfox has several differenciating advantages that explain why it is the obvious solution for many industrial, agricultural, retail, transport and smart-city projects.

As for its limits, Sigfox network's advantages are explained by the way the LPWAN communication works: devices are not connected or paired to antennas, nor do they need to signal their existence regularly (unlike cellular networks). LPWAN works on the *Aloha mechanism*: "if you have data to send, just send it!"

Antennas are constantly in "Listening mode" and devices can send data whenever they need to (the data is encrypted with an authentification key).  

This means that:
* Your device will constantly be in "Sleep mode" and only turn on when it needs to send a message. This allows Sigfox-ready devices to **consume very little energy** and function for more than 10 years with only a pair of AA batteries
* **It is impossible to jam a Sigfox communication**. To jam a signal you have to jam the receiver. As there is no pairing between the device and the antennas, the only receivers in the Sigfox network are the antennas. So, to jam a Sigfox-ready device's message you'd have to jam all the surrounding antennas.  
Some would say you still could drown the device's message in noise, but that too is impossible! As shown in the graph below, the LPWAN uses Ultra Narrow Band (UNB) signals. Sigfox's bandwidth is very narrow (hence the limited size of messages) but this means that every signal will have a very high density - a lot of energy concentrated in an ultra narrow band. A jammer cannot produce enough energy to drown those signals in noise


<p align="center">
  <img width="700" src="https://github.com/divetm/Getting-started-with-Sigfox/blob/master/images/anti-jamming_sigfox.png">
</p>


* This takes us to the third advantage: an UNB signal is powerful enough to **travel over very long distances**


<p align="center">
  <img width="300" src="https://github.com/divetm/Getting-started-with-Sigfox/blob/master/images/distances.png">
</p>


* **Sigfox also allows you to send messages to your device** (imagine wanting to change a parameter on your device: the sensitivity of a sensor, closing a valve or making a LED blink to signal acknowledgment of the message by the network...).  
That is what we call a downlink (in opposition to the uplink messages described above).  
Downlink messages cannot exceed 8 bytes, and Sigfox only allows 6 downlink messages max per device a day.  
Here you can see what UNB means and how Sigfox uses its bandwidth to send messages in both directions:

<p align="center">
  <img width="800" src="https://github.com/divetm/Getting-started-with-Sigfox/blob/master/images/frequencies.png">
</p>

###### *(RCx are the different radio regions in the world - the public radio frequency used by Sigfox in each region of the world. Your device has to be compatible with your region's radio frequency - the antennas around you will be listenning on that frequency)*

When your device wants to receive a downlink message, the device is actually the one that iniciates the communication. It will start by sending an uplink message to Sigfox's network (12 bytes max) with a special command (see more [here](#making-your-device-request-a-downlink)). Instead of going back to "sleep mode" immediately it will go into "listenning mode". The network will then send a downlink message (8 bytes max) on a randomly selected frequency of downlink agreed with the device. The device receives the downlink message, processes it however it is programmed to and will then go back to "sleep mode".

### The value chain
This is how Sigfox intervenes in your value chain:


<p align="center">
  <img width="700" src="https://github.com/divetm/Getting-started-with-Sigfox/blob/master/images/sigfox-workflow.png">
</p>


This means that:
* If antennas are in the area, they will capture it and send it to Sigfox's cloud (through an internet connection)  
* If there are no antennas in the area, the message will be lost  
However, in order to increase the chance of an antenna receiving the message, every device will send the same message three times at different moments and frequencies, enabling time and frequency diversity  

<p align="center">
  <img width="700" src="https://github.com/divetm/Getting-started-with-Sigfox/blob/master/images/uplink.png">
</p>

And, Sigfox's network is in constant deployment offering a wide coverage already, nation-wide in some countries. Check their coverage map [here](https://www.sigfox.com/en/coverage). This allows for space redundancy (when one message is sent, several antennas relay the message - your message will only appear once in Sigfox's cloud): there is only a slim chance for your message to be lost.  

Sigfox takes care of the deployment of their own antennas. Which means that you won't need to invest in antennas, their installation nor their maintenance etc.: zero CAPEX for you.  
The only expense on your side is the connectivity fee that local operators will charge you. Check the pricing for your country [here](https://buy.sigfox.com/buy).


**To sum up:**
* Sigfox invented a Radio protocol and is deploying a secured network that enables your objects to communicate (no CAPEX for you)
* The network is limited in frequency (max 140 messages per device a day) and in bandwidth (messages's size cannot exceed 12 bytes)
* This is due to how the LPWAN technology works, which also gives a lot of advantages: low energy consumption, long-distance communication and anti-jamming technology
* You can send messages in both directions between Sigfox and your device (max 4 downlink messages per device a day that cannot exceed 8 bytes in size) 

Getting started
----
### Devkits and firmware
There are a lot of options out there to get started with Sigfox and start developing your own device.
As long as you have a Sigfox module on your devkit, you're good to go. Connect whatever sensor you need and start coding.

<p align="center">
  <img width="700" src="https://clustertic.org/wp-content/uploads/2019/01/Taller-de-DevKit-Sigfox-2-1021x580.jpg">
</p>

Devkits you could use:  
https://partners.sigfox.com/products/kit

If you've used Arduino or Raspberry Pi before, there are some compatible devkits in the link above. They are pretty easy to handle and usually come with extensive documentation on how to enable the Sigfox module and how to send a message.  
[Examples using an Arduino compatible board like MKR FOX 1200](https://www.arduino.cc/en/Reference/SigFox)


<p align="center">
  <img width="250" src="https://boutique.semageek.com/5590-large_default/arduino-mkr-fox-1200-sans-antenne.jpg">
</p>

[Examples using a Raspberry Pi compatible board like SNOC's expansion board](https://github.com/SNOC/rpisigfox)


<p align="center">
  <img width="300" src="https://partners.sigfox.com/assets/medias/iv9v1y2e.jpeg">
</p>

If you have never worked with these boards before, feel free to choose what suits you best, but know that Arduino and Raspberry Pi are always good starting points for prototyping. They have a large variety of compatible sensors and a lot of available documentation.

### Sigfox Backend, Sigfox Cloud Platform, REST API and Callbacks
Once you get going, try connecting your prototypes to Sigfox's backend and check out the output. To do so here is what you need to know.
These links can be very useful:
* https://build.sigfox.com/technical-quickstart  
* https://build.sigfox.com/backend-callbacks-and-api  

A Sigfox-ready device incorporates a Sigfox radio module. The module allows it to send and receive messages from the Sigfox antennas. But, in order for that to work, the device has to be activated.  
If you buy a ready-to-use device, it will most certainly already be activated.  
If you're using a devkit (as the ones [here](#devkits-and-firmware)) then you will need to activate it. Follow the very simple steps listed here: https://support.sigfox.com/docs/sigfox-activate:-when-and-how-to-use-it

Your device will have an ID that it uses to identify itself on the network (check for a label on the back of your device with a mention of an ID).  
It also uses a second number called Porting Authorization Code (PAC) that you can probably find following [these](https://support.sigfox.com/docs/device-idpac-couple) instructions.

These identifiers will come in handy if you want to register your device on Sigfox's backend.
[This document](https://storage.sbg1.cloud.ovh.net/v1/AUTH_669d7dfced0b44518cb186841d7cbd75/staging_docs/att10756424-Customer%20helpbook_2017.pdf) explains what Sigfox's backend is and how to use it.  
TLDR: It basically allows you to manage all your connected devices, connect new ones and manage how messages received from/sent to your devices should be handled (more on this [here](#visualizing-your-data-and-sending-instructions-callbacks-and-downlinks)).

Most of your Sigfox-ready devices will come with an app or website that allows you to visualize the data sent by the device. In that case, you don't need Sigfox's backend, since the manufacturer will have taken care of that.

<p align="center">
  <img width="700" src="http://52.17.73.150/wp-content/uploads/2016/10/sigfox-network.jpg">
</p>

###### *The devices send messages received by antennas > The antennas send the data to Sigfox's cloud (Sigfox's backend allows you to manage what happens from there on) > The data is usually then sent to data visualization apps or trigger an action or a response (downlink)*

If your device doesn't come with an app and you need to build one yourself, [this](https://storage.sbg1.cloud.ovh.net/v1/AUTH_669d7dfced0b44518cb186841d7cbd75/staging_docs/att10756424-Customer%20helpbook_2017.pdf) is how you do it.  
You will have to setup callbacks or use Sigfox's REST API. Learn more on that [here](https://build.sigfox.com/backend-callbacks-and-api).

Once that is set up you will be able to see your messages being sent and decide what Sigfox should do with them (like a manufacturer would do to build their own app).

### Using a platform to design your data workflow
One of the things you could do with the data sent by your devices is building a dashboard to visualize them.
Two platforms can be very useful for that: Losant and Ubidots. There are many other platforms that can do this, feel free to use the one that you prefer.  
By setting up a callback on your Sigfox's backend, these platforms will allow you to process the data sent by your device. For that you will need to know how to parse and decode the data contained in the payload sent by your device (received by the platform).  
Use one of these plarforms to build your first complete application with decoding, workflows and data visualization.

[How to build a dashboard on Losant to visualize data received through Sigfox callbacks](https://www.losant.com/blog/collect-monitor-visualize-sigfox-data)  
[How to do that with Ubidots](https://help.ubidots.com/technical-resources/setup-your-sigfox-callback-to-talk-with-ubidots-cloud)

### Making your device request a downlink
These platforms also allow you to send a downlink message as a response to a callback. To setup that kind of downlink messages  follow [these steps](https://github.com/sigfox/sigfox-downlink).  
In Losant you can describe what the downlink message should be in the "Webhook reply" block.  
For Ubidots, everything is explained [here](https://help.ubidots.com/developer-guides/ubifunctions-manage-downlink-messages-with-sigfox-and-ubidots).

In [this doc](https://github.com/sigfox/sigfox-downlink#set-up-your-downlink-callback) you can see the mention of a "Direct" Downlink mode. If you use that, the data you insert in the "Downlink data in hexa" field will be sent to any device of that Device type that asks for a Downlink message at any time (remember you can only send 6 downlink messages a day per device - and they can't exceed 8 bytes in size).

Remember that a Downlink message will only be sent to your device if your device sent a message with a request for such a response first?  
So, you probably what to know how to make your device request a Downlink message now. It might seem a bit technical but it is actually very simple.  
For this, you need to know that Sigfox modules usually work with a language that understands what we call *AT Commands*. This means that if you want to communicate with the module directly you will have to send instructions under the AT command format. For example:
* "AT$RC" will allow you to reset the module's channel  
* "AT$SF=..." will order the module to send whatever comes after the equal sign through Sigfox's network  
You can usually check what commands are available in your module's documentation (like [this one](https://www.onsemi.com/pub/Collateral/AX-SIGFOX-D.PDF) for the Wisol module AX-Sigfox). Some, like the Wisol module, will give you there ID and PAC with the commands "AT$I=10" and "AT$I=11" for example.

In any case, if you want to receive a downlink response to a message sent by your device, you just have to add ",1" to the 12-bytes-max payload you send with the command "AT$SF=..." (e.g. "AT$SF=4d6174746869657542657374,1" will send the payload "4d6174746869657542657374" to Sigfox's network and request a downlink response).
If you set up the downlink parameter in your backend correctly (as explained above), the device will receive the downlink you wanted it to.

Building your own prototype
----
### Methodology
TODO

### The hardware
TODO

### The firmware
TODO

### Using Losant to design your data workflow
TODO

Conclusion
----
TODO (Consider data visualization, data analysis and iteration: Agile methodology. Goal of IoT: gain access to data previously unaccessible to optimize processes through data analysis)

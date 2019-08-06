# Building your first Sigfox connected prototype
*Learn how to build your own Sigfox-connected device with a devkit and a couple of components*

Introduction
----
The goal of this repo is to learn how to build your own IoT device prototype using a Sigfox-connected devkit and a couple of components.  
If you want to learn more about Sigfox's technology, don't hesitate to checkout [this repo](https://github.com/divetm/Getting-started-with-Sigfox).  
Here, we will focus on getting you ready to build your first Sigfox-connected prototype from scratch.

1. [A quick overview of what Sigfox is](#a-quick-overview-of-what-sigfox-is)  
  a. [The technology](#the-technology)  
  b. [Its limits and its advantages](#its-limits-and-its-advantages)  
  c. [The value chain](#the-value-chain)

2. [Getting started](#getting-started)  
	a. [Devkits and firware](#devkits-and-firmware)  
	b. [Using Sigfox](#using-sigfox)  

3. [Sens'it](#sensit)  
  a. [The device](#the-device)  
  b. [SDK and setup](#sdk-and-setup)  
  c. [Troubleshoot](#troubleshoot)  

4. [Build your own application with Sens'it](#build-your-own-application-with-sensit)  
	a. [Presentation](#presentation)  
	b. [Main](#main)  
	c. [discovery.c](#discoveryc)  
	d. [discovery.h](#discoveryh)  
	e. [Makefile](#makefile)  
	f. [Creating a dashboard in Losant](#creating-a-dashboard-in-losant)


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
However, it is important to understand that the payload (data) that can be sent over Sigfox's network is limited in:  
* size (maximum 12 bytes) - this is intrinsic to how the LPWAN technology works, we will come back to that;
* frequency (140 messages a day max) - that is due to the fact that Sigfox uses a public radio frequency and cannot hog it

### Its limits and its advantages
jbadfn

### The value chain

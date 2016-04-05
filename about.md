---
layout: page
title: About
permalink: /about/
weight: 2
---

The Ampehre project is a BSD-licensed modular software framework used to sample various types of sensors embedded in integrated circuits or on circuit boards
deployed to servers with a focus to heterogeneous computing. The project is developed at the [Computer Engineering Group of the University of Paderborn](https://cs.uni-paderborn.de/ceg/).

It enables accurate measurements of power, energy, temperature, and device utilization for computing resources such as CPUs (Central Processing Unit),
GPUs (Graphics Processing Unit), FPGAs (Field Programmable Gate Array), and MICs (Many Integrated Core) as well as system-wide measuring via IPMI (Intelligent Platform Management Platform).
For this, no dedicated measuring equipment such as DMMs (Digital Multimeter) is needed.

We have implemented the software in a way that the influence of the measuring procedures running as a multi-threaded CPU task have a minimum impact to the overall CPU load.
The modular design of the software facilitates the integration of new resources.
Though it has been enabled to integrate new resources since version v0.5.1, the effort to do so is still quite high.
Accordingly, our plans for the next releases are broader improvements on the resource integration as well as an extensive project review to stabilize the code base.

You can find the source code for Ampehre at [github.com/akiml/ampehre](https://github.com/akiml/ampehre)

This work was partially supported by the German Research Foundation (DFG) within the Collaborative Research Centre "On-The-Fly Computing" ([SFB 901](http://sfb901.uni-paderborn.de)) and the European Union Seventh Framework Programme under grant agreement no. 610996 ([SAVE](http://www.fp7-save.eu)).

<center>
<a href="http://sfb901.uni-paderborn.de"><img src="{{ site.baseurl }}/assets/sfb901_logo.png" alt="SFB901 Logo" width="150" height="63" hspace="50"></a>
<a href="http://www.fp7-save.eu"><img src="{{ site.baseurl }}/assets/save_logo.png" alt="SAVE Logo" width="77" height="67" hspace="50"></a>
</center>

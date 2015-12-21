---
layout: post
title:  "Major Release"
date:   2015-12-21
categories: ampehre update
brief: Ampehre v0.6.1 with lower overhead is now available.
--- 
---

####Description:

Ampehre, v0.6.1 and the updated documentation, is now available at our github repository [github.com/akiml/ampehre](https://github.com/akiml/ampehre).

---

####Bugfixes:
- libmeasure does not abort anymore because of IPMI timeouts.

####Feature improvements:
- New json-printer in hettime to print results into json files.
- Add ioctl call to measure driver to set the IPMI timeout at the start of the measuring system.
- Add C++ template to select light or full library variant.
- New parameter check_for_exit_interrupts to allow undelayed library terminations.
- Adapted interfaces with more options. New interfaces are not downward compatible.

---


Check out the Ampehre [documentation][docs] for more informations about the current version.

[docs]: {{site.baseurl}}/documentation/

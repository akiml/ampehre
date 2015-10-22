---
layout: post
title:  "Performance Enhancement Release"
date:   2015-10-21
categories: ampehre update
brief: Ampehre v0.5.11 with better performance is now available.
--- 
---

####Description:

The latest version of Ampehre, v0.5.11, is now available at our github repository [github.com/akiml/ampehre](https://github.com/akiml/ampehre).

---

####Bugfixes:
- Remove unnecessary mutex in measuring threads.
- Fix a couple of memory leaks.

####Feature improvements:
- Adjust timeout parameter of the IPMI requests in the measure driver for more stability (needed since iDRAC7 update 2.20.20).
- Adjust default refresh rate in msmonitor to decrease CPU utilization.
- New ringbuffer datastructure in msmonitor.
- Writing measurements plotted in msmonitor to a CSV file is an optional feature now. You can turn this feature on/off in the settings window.

---


Check out the Ampehre [documentation][docs] for more informations about the current version.

[docs]: {{site.baseurl}}/documentation/

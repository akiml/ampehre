---
layout: post
title:  "Performance Enhancement Release!"
date:   2015-10-21
categories: ampehre update
brief: Ampehre v0.5.11 with better performance is now available!
--- 
---

####Description:

The latest version of Ampehre, v0.5.11, is now available at our github repository [github.com/akiml/ampehre](https://github.com/akiml/ampehre).

---

####Bugfixes:
- Fix unnecessary mutex in measuring threads.

####Feature improvements:
- Adjust parameters of the IPMI requests in the measure driver for more stability.
- Adjust default refresh rate in msmonitor for less CPU utilization.
- New ringbuffer datastructure in msmonitor.
- Control the output of msmonitor to a csv file via the GUI. You can find this option in the menu under File - Settings or Ctrl-S. Per default no csv-file will be created.
- Checked hettime and msmonitor for memory leaks.

---


Check out the Ampehre [documentation][docs] for more informations about the current version.

[docs]: {{site.baseurl}}/documentation/

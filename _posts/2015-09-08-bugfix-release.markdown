---
layout: post
title:  "Bugfix Release"
date:   2015-09-08
categories: ampehre update
brief: Ampehre v0.5.10 is now available.
--- 
---

#### Description:

The latest version of Ampehre, v0.5.10, is now available at our github repository [github.com/akiml/ampehre](https://github.com/akiml/ampehre).

---

#### Bugfixes:
- Fix GPU frequency settings in ms_hetsched example application.
- Fix hetpowidle JSON file writer.
- Fix headers of CSV files created by hettime's CSV file printer.

#### Feature improvements:
- Adjust sampling rates in hetpowidle tool.
- Add MIC support to task wrapper and adjust its default sampling rates.
- Adjust libmeasure light version.
- Print a message whether light or full version of libmeasure is used. Note that you can change the used library (light or full version) and the corresponding plugins by overwriting the default linkage. For this, you must set the shell variable LD_PRELOAD before executing any application.

---


Check out the Ampehre [documentation][docs] for more informations about the current version.

[docs]: {{site.baseurl}}/documentation/

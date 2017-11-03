---
layout: post
title:  "Major Release"
date:   2017-11-03
categories: ampehre update
brief: Ampehre v1.0.0 introduces the APAPI library to make use of the PAPI library for measurement.
--- 
---

#### Description:

Ampehre, v1.0.0 is now available at our github repository [github.com/akiml/ampehre](https://github.com/akiml/ampehre).

---

#### Feature improvements:

The APAPI library wraps the well-known [Performance Application Programming Interface (PAPI)][papi] library and implements the libmeasure interface.
This way applications using libmeasure, such as our hettime tool, are able to make use of PAPI.

APAPI depends on several extensions that we made to PAPI components.
Our changes can be found in the *papi* branch, that is also included as a submodule in the *papi* folder.
When cloning our repository make sure to add submodules using
```
git clone --recursive [...]
```

To use the library instead of libmeasure force the loading of *libms_common_apapi.so*.
For example to let hettime use the library start hettime this way:
```
LD_PRELOAD=/path/to/libms_common_apapi.so hettime [...]
```

For more information about the usage of the APAPI library check the [readme file in the APAPI subfolder][apapireadme].

---


Check out the Ampehre [documentation][docs] for more informations about the current version.

[docs]: {{site.baseurl}}/documentation/
[papi]: <https://icl.utk.edu/papi/>
[apapireadme]: <https://github.com/akiml/ampehre/tree/master/apapi>

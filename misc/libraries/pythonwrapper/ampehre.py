#!/usr/local/bin/python3

#
# ampehre.py
# 
# Copyright (C) 2016, Achim Lösch <achim.loesch@upb.de>
# All rights reserved.
# 
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
# 
# encoding: UTF-8
# tab size: 4
# 
# author: Achim Lösch (achim.loesch@upb.de)
# created: 2/07/17
# version: 0.7.5 - add a python wrapper for simple measuring by python scripts
#

from ctypes import *
import time

class AmpehreWrapper:
	def __init__(self):
		self.libAmpehre = cdll.LoadLibrary("/usr/ampehre/lib/libpythonmeasure.so")
		
		self.libAmpehre.ampehre_get_energy_total.restype	= c_double
		self.libAmpehre.ampehre_get_v2freq_total.restype	= c_double
		self.libAmpehre.ampehre_get_pstate_total.restype	= c_uint64
		
	def init(self):
		self.libAmpehre.ampehre_init()
	
	def start(self):
		self.libAmpehre.ampehre_start()
	
	def stop(self):
		self.libAmpehre.ampehre_stop()
	
	def fini(self):
		self.libAmpehre.ampehre_fini()
	
	def getEnergyTotal(self):
		return float(self.libAmpehre.ampehre_get_energy_total())
	
	def getV2FreqTotal(self):
		return float(self.libAmpehre.ampehre_get_v2freq_total())
	
	def getPStateTotal(self):
		return int(self.libAmpehre.ampehre_get_pstate_total())

def main():
	ampehre = AmpehreWrapper()
	
	ampehre.init()
	ampehre.start()
	
	time.sleep(10)
	
	ampehre.stop()
	
	print("Consumed total energy: " + str(ampehre.getEnergyTotal()))
	
	ampehre.fini()

main()

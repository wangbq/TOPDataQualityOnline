#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# --------------------------------------------------------------------
# sroot to root conversion
# use -i and -o to specify input and output file names
# ex.: basf2 sroot2root.py -i <path/input_file> -o <path/output_file>
# --------------------------------------------------------------------

from basf2 import *

# Create path
main = create_path()

# Input
evtinput = register_module('SeqRootInput')
main.add_module(evtinput)

# Event info printer
# main.add_module('EventInfoPrinter', logLevel=LogLevel.INFO)

# Output
output = register_module('RootOutput')
main.add_module(output)

dqmmodule = register_module("TOPDataQualityOnline")
dqmmodule.param('framesPerEvent',1)
main.add_module(dqmmodule)

# Show progress of processing
main.add_module('Progress')

# Process events
process(main)

print(statistics)


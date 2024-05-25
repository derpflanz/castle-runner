#!/bin/bash

make -C GameCodeNG emulate
make -C VirtualCR001

VirtualCR001/virtual_cr001 GameCodeNG/generated/castlerunner.hex GameCodeNG/generated/castlerunner-data.hex 
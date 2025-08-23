#!/bin/bash

tos.py build
echo 1 | tos.py flash
echo 2 | tos.py monitor

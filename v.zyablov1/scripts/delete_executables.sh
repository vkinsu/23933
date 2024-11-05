#!/bin/bash

find . -type f -executable -path "*/task[0-9]*/*" -print -exec rm -f {} +

echo ""


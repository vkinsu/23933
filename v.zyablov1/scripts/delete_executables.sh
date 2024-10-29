#!/bin/bash

find . -type f -executable -path "*/task[0-9]*/*" -exec rm -f {} +

echo ""

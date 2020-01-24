#!/bin/sh
cd "$(/Users/Heyseb1/Documents/D2/processing_data "$0")";
CWD="$(pwd)"
echo $CWD
python download_gdrive.py

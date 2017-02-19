#!/bin/bash

if [ "${1}" == "" ];then
    size=52
else
    size=${1}
fi
dd if=/dev/urandom bs=${size} status=none count=1 | base64 | tr '[A-Za-z]' '[N-ZA-Mn-za-m]' | tr -d '='

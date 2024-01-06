#!/bin/bash

for tracked_var in $AUTO_ENV_TRACKED_VARS; do
    echo "Removed ${tracked_var}"
    unset $tracked_var
done
echo "Goobye"

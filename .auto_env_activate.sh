#!/bin/bash

# Store current environment variables
OLD_ENV_VARS=$(env | grep -v '^_' | cut -d= -f1)

# Source the script
source environment/envr.vars

# Find the new environment variables
NEW_ENV_VARS=$(env | grep -v '^_' | cut -d= -f1)


# Collect new variables into a string
EXPORT_VARS=""

for VAR in $NEW_ENV_VARS; do
    if [[ $OLD_ENV_VARS != *$VAR* ]]; then
        echo "Added ${VAR}"
        EXPORT_VARS="$EXPORT_VARS $VAR"
    fi
done

# Export the collected variables
export AUTO_ENV_TRACKED_VARS="$EXPORT_VARS"

#Venv
VENV_PATH=".venv"

if [ -d "$VENV_PATH" ]; then
    source "$VENV_PATH/bin/activate"
    echo "Virtual environment activated."
else
    echo "Error: Virtual environment not found."
fi

echo "Welcome to KBv.3"

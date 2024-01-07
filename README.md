# KBv.3
Firmware for KBv.3

## Initial Setup
* Install [arm-none-eabi_gcc](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (v13.2)
* For auto_env support, add the provided ```environment/auto_env``` to
  your .bashrc file.
* Update the paths.local file to point to the correct locations.
* Create virtual environment ```python3.10 -m venv .venv```
* Activate virtual environment ```source .venv/bin/activate```
> Note: auto _env will activate and deactivate the venv in the future
* Install required packages ```python -m pip install -r environment/requirements.txt```
* Install required pyocd packs ```pyocd pack -i ATSAMD21J18A```

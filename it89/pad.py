# create_flash_image.py
import os

# Filenames
firmware_file = "./it8951.bin"
waveform_file = "./320_R467.wbf"
output_file = "./flash.bin"

# Constants
FIRMWARE_OFFSET = 0x000000          # Firmware starts at 0
WAVEFORM_OFFSET = 0x100000 
FLASH_SIZE =      0x400000         # Waveform starts at 1 MB

# Read firmware
with open(firmware_file, "rb") as f:
    firmware = f.read()

# Read waveform
with open(waveform_file, "rb") as f:
    waveform = f.read()

# Prepare padded flash image
# flash_size = WAVEFORM_OFFSET + len(waveform)
flash_image = bytearray([0x00] * FLASH_SIZE)

# Copy firmware into start
flash_image[0:len(firmware)] = firmware

# Copy waveform at 0x100000
flash_image[WAVEFORM_OFFSET:WAVEFORM_OFFSET+len(waveform)] = waveform

# Write output file
with open(output_file, "wb") as f:
    f.write(flash_image)

print(f"Flash image created: {output_file}")
print(f"Firmware: 0x000000 - 0x{len(firmware):06X}")
print(f"Waveform: 0x{WAVEFORM_OFFSET:06X} - 0x{WAVEFORM_OFFSET + len(waveform):06X}")
print(f"Total size: {len(flash_image)} bytes")

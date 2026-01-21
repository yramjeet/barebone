import binascii

with open('./it8951.bin', 'rb') as f:
    while True:
        chunk = f.read(16)
        if not chunk:
            break
        hex_str = ' '.join(f'{b:02X}' for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in chunk)
        print(f'{hex_str:<48} {ascii_str}')
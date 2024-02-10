import sys

# ff ff 00 00 00 00 00

asciifile = "bytes.asc"
binfile = "../GameCode/castlerunner-data.hex"
print("Paste ascii encoded data.")

line = ''
with open(asciifile, "r") as fp:
    line = fp.read()

print(f"Read {len(line)} bytes from {asciifile}")

try:
    b = bytes.fromhex(line)
    with open(binfile, "wb") as fp:
        written = fp.write(b)

    print(f"Wrote {written} bytes to {binfile}")
except ValueError as e:
    print(f"Failed to read hex: {e}")

import binascii
import sys

with open(sys.argv[1], "rb") as f:
    b = f.read()

hex = binascii.hexlify(b).decode()
print(hex)

for i in range(0, len(hex), 16): 
    word = hex[i: i+16]
    word = word[::-1]
    print(word)

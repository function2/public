#!/usr/bin/env python3
# Simple (SLOW) program to calculate the SHA-256.
# Copyright 2015, Michael Seyfert <michael@codesand.org>
# http://codesand.org
# License GPL v2

import sys, struct

K = [
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
]
W = [0x0]*64

def rot(x,n):
    return (x >> n) | ((x & ((1<<n)-1))<<(32-n))

def Ch(x,y,z):
    return (x & y) ^ (~x & z)

def Maj(x,y,z):
    return (x & y) ^ (x & z) ^ (y & z)

def Ea(x):
    return rot(x,2) ^ rot(x,13) ^ rot(x,22)

def Eb(x):
    return rot(x,6) ^ rot(x,11) ^ rot(x,25)

def ea(x):
    return rot(x,7) ^ rot(x,18) ^ (x >> 3)

def eb(x):
    return rot(x,17) ^ rot(x,19) ^ (x >> 10)

def compression_func(r,j):
    t1 = r[7] + Eb(r[4]) + Ch(r[4],r[5],r[6]) + K[j] + W[j]
    t2 = Ea(r[0]) + Maj(r[0], r[1], r[2])
    r[7] = r[6]
    r[6] = r[5]
    r[5] = r[4]
    r[4] = (r[3] + t1) & 0xffffffff
    r[3] = r[2]
    r[2] = r[1]
    r[1] = r[0]
    r[0] = (t1 + t2) & 0xffffffff

def sha256(b):
    """
    Compute the SHA-256 of input variable 'b' which is of builtin type
    bytes (a string of bytes).
    Returns the hash as a list[] of 8 unsigned (32-bit) integers.
    """
    l = 8 * len(b) # l is the length of the message, in bits.
    k = (448 - (l + 1)) % 512 # python uses the mathematical modulo definition.
    # Add to b to fill the input bits to a multiple of 512 bits.
    b += (b'\x80' + bytes(int((k-7)/8)) + struct.pack(">q", l))
    #initial H values.
    h = [0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
         0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19]
    m = [0]*16 # holds the next 512 bits of the input message.
    r = [0]*8 # 8 registers used.
    # The message is read 64 bytes (512 bits) each iteration.
    N = len(b)//64
    for i in range(N):
        # Set the 16 32-bit M values. These are M_j^(i) (0 <= j <= 15)
        for j in range(16):
            m[j] = struct.unpack(">I",b[i*64+4*j : i*64+4*j+4])[0]
        # Initialize the registers with the (i-1)st intermediate hash value.
        for j in range(8):
            r[j] = h[j]
        # Setup the expanded message blocks W[0] to W[63].
        for j in range(16):
            W[j] = m[j]
        for j in range(16,64):
            W[j] = (eb(W[j-2]) + W[j-7] + ea(W[j-15]) + W[j-16]) & 0xffffffff
        #  Apply the compression functions to update the registers, from j=0 to 63.
        for j in range(64):
            compression_func(r,j)
            # # Debug print
            # print(hex(r[0]),hex(r[1]),hex(r[2]),hex(r[3]),
            #       hex(r[4]),hex(r[5]),hex(r[6]),hex(r[7]))
        # Compute the new hash value.
        for j in range(8):
            h[j] = (r[j] + h[j]) & 0xffffffff
    return h

if __name__ == "__main__":
    h = sha256( open(sys.argv[1], "rb").read() )
    print("".join([hex(h[k])[2:]+' ' for k in range(8)]))

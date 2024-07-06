# lsmaps print RAM and Swap usage of process maps

Goes through memory mapped regions of a process and prints each regions RAM and Swap usage

```
# lsmaps `pidof chrome` | grep chrome
VSZ   RAM   SWAP	Line
41MB  9MB   0B  	5dd7f4941000-5dd7f72f4000 r--p 00000000 103:07 11534454                  /opt/google/chrome/chrome
179MB 51MB  0B  	5dd7f72f5000-5dd802677000 r-xp 029b3000 103:07 11534454                  /opt/google/chrome/chrome
9MB   9MB   0B  	5dd802677000-5dd802fdd000 r--p 0dd34000 103:07 11534454                  /opt/google/chrome/chrome
80KB  80KB  0B  	5dd802fde000-5dd802ff2000 rw-p 0e69a000 103:07 11534454                  /opt/google/chrome/chrome
4KB   4KB   0B  	5dd802ff2000-5dd802ff3000 r--p 0e6ae000 103:07 11534454                  /opt/google/chrome/chrome
```

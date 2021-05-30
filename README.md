# sign-jetson-dtb
Sign dtb for Jetson TX2

Jetson TX2 need dtb was "signed" in emmc partiion.
Default key is 16 bytes of zero. 
(Orig project: https://github.com/kmartin36/nv-tegra-sign (python))

#### Dependencies:
```
libssl-dev
```
#### Building:
```
make
```
#### Usage:
```
# sign dtb (on tx2 or not)
./sign-jetson-dtb 1.dtb s1.dtb
# write dtb on tx2
cat s1.dtb > /dev/mmcblk0p30
cat s1.dtb > /dev/mmcblk0p31
```


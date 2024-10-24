# hot_reset_pcie
this repository contains a way to make and test a hot reset pcie for FPGAs


# USE BASH SCRIPT

```plaintext
bash safe-remove.sh 01:00.0
```

# COMPILE TEST DRIVERS

```plaintext
mkdir build
cd build
cmake ..
make
./test_hot_reset
```
# kick punch it's all in the mind

parappa 1 decomp (september 24 prototype)

## building is easy

```
$ sudo apt update
$ sudo apt install git make binutils-mips-linux-gnu cpp-mips-linux-gnu python3 python3-pip python3-venv
$ git clone --recursive https://github.com/EstexNT/parappa1.git
$ cd parappa1
$ python3 -m venv .venv
$ source .venv/bin/activate
$ python3 -m pip install -r requirements.txt
```

then copy your extracted disk into `disc` and run `make clean-check`


don't forget to `source .venv/bin/activate` everytime you open a new terminal


## makeing 

- `build` build game 
- `check` build game and compare checksum
- `clean` clear build dir
- `clean-build` clear config and build game
- `clean-check` clear config and build game and compare checksum


## hey where are the overlays

later

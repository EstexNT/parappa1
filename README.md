# PaRappa the Rapper 1 Decompilation

This is a decompilation of PaRappa the Rapper (Sep 24, 1996 prototype) for the Sony PlayStation.

## Building instructions

The following commands will install the requirements, clone the repository and setup a Python virtual environment:
```
$ sudo apt update
$ sudo apt install git make binutils-mips-linux-gnu cpp-mips-linux-gnu python3 python3-pip python3-venv
$ git clone --recursive https://github.com/EstexNT/parappa1.git
$ cd parappa1
$ python3 -m venv .venv
$ source .venv/bin/activate
$ python3 -m pip install -r requirements.txt
```

Afterwards, copy your extracted disk into `disc` and run `make clean-check`.

**Note:**
Everytime you open a new terminal, you will have to run `source .venv/bin/activate` inside the repo directory.


## Make Rules 

- `build` Build game 
- `check` Build game and compare checksum
- `clean` Clear build dir
- `clean-build` Clear config and build game
- `clean-check` Clear config and build game and compare checksum


# mset

A cli tool for setting system-wide mouse sensitivity and acceleration on macOS.

## Usage

```bash
mset [-s <sensitivity>] [-a <acceleration>]
```

## Options

- `-s <sensitivity>`: Set mouse sensitivity, default is 190, range is 1-199
- `-a <acceleration>`: Set mouse acceleration, default is 0, range is 0-10000000, 0 means disable acceleration
- `-d`: Run as daemon, will check and re-apply mouse settings if system settings are changed or affected by other programs
- `-h`: Show help

## Example

```bash
mset                  # set sensitivity to 190 and disable acceleration
mset -s 180           # set sensitivity to 180 and disable acceleration
mset -s 100 -a 50000  # set sensitivity to 100 and acceleration to 50000
```

## Build from source

Dev dependencies:

- clang
- make

Install dependencies:

```bash
xcode-select --install # or brew install llvm

brew install make
```

### Build

```bash
make
```

### Install

```bash
sudo make install
```

### Uninstall

```bash
sudo make uninstall
```

## License

This project is licensed on the [MIT License](LICENSE).

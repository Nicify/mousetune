# mset

A cli tool for setting system-wide mouse sensitivity and acceleration on macOS.

## Usage

```bash
# sensitivity: 1-199
# acceleration: 0-10000000
mset [sensitivity] [acceleration]
```

## Example

```bash
# Set sensitivity to 190 and disable acceleration.
mset 190

# Set sensitivity to 100 and acceleration to 30000.
mset 150 30000
```

## Build from source

### Dev dependencies

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

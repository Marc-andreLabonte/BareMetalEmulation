# Bare Metal Emulation

Workshop @ NSEC 2025

# Setup instructions

```bash
git clone https://github.com/Marc-andreLabonte/BareMetalEmulation
```

# Installation

## As root:

Installer docker engine

```bash
apt install docker.io
```

Add your user to docker group (or invoke docker with sudo)

### If docker would require a proxy on your computer:

Set proxy IP address and port in http-proxy.conf file

```bash
mkdir /etc/systemd/system/docker.service.d/
cp http-proxy.conf /etc/systemd/system/docker.service.d/
systemctl daemon-reload
systemctl restart docker
```

## As user:

### If a proxy is required:

Set proxy IP address and port in config.json file

Then, copy config.json in $HOME/.docker

```bash
mkdir ~/.docker
cp config.json ~/.docker/
```

build docker container

```bash
docker build --rm --tag workshop2025 .
```

Run docker container

```bash
docker run -it --rm -p 8888:8888 workshop2025
```

Download Ghidra or similar tool
    - [https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_11.3.2_build/ghidra_11.3.2_PUBLIC_20250415.zip](https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_11.3.2_build/ghidra_11.3.2_PUBLIC_20250415.zip)



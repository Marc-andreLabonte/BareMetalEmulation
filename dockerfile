FROM quay.io/jupyter/base-notebook
USER root
# Install i386 support
RUN apt-get update && \
    apt-get install -y git bash gdb-multiarch make \
         # we also need to fetch qemu sources for that version
         qemu-system \
         # install cross compilers
         gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf \
         gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu \
         gcc-arm-none-eabi binutils-arm-none-eabi \
         # pwndbg needs some dependencies
         gdbserver python3-dev python3-venv python3-setuptools libglib2.0-dev libc6-dbg curl \
         # telnet clients to connect to qemu monitor
         telnet ncat \
         # Hint: tools required for exercise 4
         file binwalk squashfs-tools \
         # Also need device-tree-compiler to convert flattened device tree blobs in human readable form.
         device-tree-compiler

# also needed for pwndbg
RUN dpkg --add-architecture i386 || true
RUN apt-get install -y libc6-dbg:i386 libgcc-s1:i386 || true


# apt might need a proxy
#COPY 51proxy /etc/apt/apt.conf.d/51proxy


USER jovyan
# Install in the default python3 environment
RUN pip install --no-cache-dir 'flake8' && \
    fix-permissions "${CONDA_DIR}" && \
    fix-permissions "/home/${NB_USER}"


#COPY --chown=${NB_UID}:${NB_GID} env_vars.sh /home/${NB_USER}/env_vars.sh
#RUN . ./env_vars.sh

#RUN git clone -q --depth 1 https://github.com/pwndbg/pwndbg
RUN git clone --branch 2025.01.20 --single-branch -q  https://github.com/pwndbg/pwndbg
# patch setup.sh so it does not invoke sudo to install dependencies (already installed)
COPY --chown=${NB_UID}:${NB_GID} pwndbg/setup.sh /home/${NB_USER}/pwndbg/setup.sh
# remove pt dependency from pyproject.toml, crash install otherwise
#COPY --chown=${NB_UID}:${NB_GID} pwndbg/pyproject.toml /home/${NB_USER}/pwndbg/pyproject.toml
# launch pwndbg install script
RUN cd pwndbg && \
  ./setup.sh


# Install from the requirements.txt file
#COPY --chown=${NB_UID}:${NB_GID} requirements.txt /tmp/
#RUN pip install --no-cache-dir --requirement /tmp/requirements.txt && \
#    fix-permissions "${CONDA_DIR}" && \
#    fix-permissions "/home/${NB_USER}"

COPY --chown=${NB_UID}:${NB_GID} ex1 /home/${NB_USER}/ex1
COPY --chown=${NB_UID}:${NB_GID} ex2 /home/${NB_USER}/ex2
COPY --chown=${NB_UID}:${NB_GID} ex3 /home/${NB_USER}/ex3
COPY --chown=${NB_UID}:${NB_GID} ex4 /home/${NB_USER}/ex4
COPY --chown=${NB_UID}:${NB_GID} solutions /home/${NB_USER}/solutions

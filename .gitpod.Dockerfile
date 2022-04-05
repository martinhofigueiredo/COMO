FROM gitpod/workspace-full

# Install custom tools, runtime, etc.
RUN sudo apt-get update \
    && sudo apt-get install mininet xterm -y \
    && sudo rm -rf /var/lib/apt/lists/* \
    && sudo pip3 install pandoc
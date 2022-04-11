FROM gitpod/workspace-full

# Install custom tools, runtime, etc.
RUN sudo apt-get update \

    && sudo apt-get install mininet xterm -y \
    && sudo apt-get install openvswitch-switch \
    && sudo service openvswitch-switch start \
    && sudo rm -rf /var/lib/apt/lists/* \
    && sudo pip3 install pandoc
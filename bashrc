# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
    . /etc/bashrc
fi

# User specific aliases and functions
export PS1="\[\e[00;33m\]\u\[\e[0m\]\[\e[00;37m\]@\h:\[\e[0m\]\[\e[00;36m\][\w]\[\e[0m\]\[\e[00;37m\]\n\[\e[0m\]\[\e[00;36m\]:\[\e[0m\]"
#source /usr/usc/mpich2/1.3.1..10/setup.sh
#source /usr/usc/cuda/default/setup.sh
source /usr/usc/gnu/gcc/4.8.1/setup.sh
source /usr/usc/opencv/2.4.8/setup.sh
source /usr/usc/openmpi/default/setup.sh
export PKG_CONFIG_PATH=/usr/usc/opencv/2.4.8/lib/pkgconfig/

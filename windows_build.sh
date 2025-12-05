set -eux

GLFW=$HOME/glfw-3.4/src/libglfw3.a make -j$(nproc)

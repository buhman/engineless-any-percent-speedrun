set -eux

make \
    GLFW=$HOME/glfw-3.4/src/libglfw3.a \
    -B -j$(nproc)
mv main.exe engineless-any-percent-speedrun.x86_64.exe

#make \
#    GLFW=$HOME/glfw-3.4-i386/src/libglfw3.a \
#    I386=1 \
#    -B -j$(nproc)
#mv main.exe engineless-any-percent-speedrun.x86_32.exe

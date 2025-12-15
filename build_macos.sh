make \
    TARGET='-target x86_64-apple-macos10.15' \
    GLFW=$HOME/glfw-3.4-x86_64/src/libglfw3.a \
    -j -B
mv main engineless-any-percent-speedrun.x86_64-macos10.15
zip engineless-any-percent-speedrun.x86_64-macos10.15.zip engineless-any-percent-speedrun.x86_64-macos10.15

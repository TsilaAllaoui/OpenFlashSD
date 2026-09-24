find . \
    \( \
        -path './lib' \
        -o -path './build' \
        -o -path './graphics' \
        -o -path './src/generated' \
        -o -path './include/generated' \
        -o -path './tools' \
    \) -prune \
    -o \( \
        -name '*.cpp' \
        -o -name '*.h' \
        -o -name '*.hpp' \
    \) \
    -type f \
    -exec clang-format -i {} +
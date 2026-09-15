arm-none-eabi-nm -C -S --size-sort openflashsd.elf |
awk '$1 ~ /^08/ {
    size = strtonum("0x"$2);
    name = substr($0, index($0,$3));
    printf "%8d %8.2f KB  %s\n", size, size/1024, name
}' |
sort -nr |
head -50
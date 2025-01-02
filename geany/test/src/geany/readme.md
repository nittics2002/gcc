#

test11 Extensionのコンパイルは通ったけど、プラグインマネジャに表示しないので失敗


##

```bash

pwd
#.......gcc/geany/test/src/geany/

gcc -c test02.c -fPIC $(pkg-config --cflags geany) -I.
gcc test02.o -shared $(pkg-config --libs geany) -o test02.so
cp test02.so ~/.local/usr/lib/geany/plugins/

```


# ctags比較 geany標準ctagsとgeanyctags.plugin

## ctags生成コマンド

### geany標準

```bash
ctags -n --fields=EfiklsZSt -R -o geany.c.tags .

```
### plugin

``` bash
find -L . -not -path '*/\.*' -type f | \
	ctags --totals --fields=fKsSt \
		--extra=-fq --c-kinds=+p \
		--sort=foldcase --excmd=number \
		-L - -f geany.tags'
```

## 比較

geany標準-plugin

value	readtags.h	117;"	m	language:C++	scope:struct:__anona3d896660808	typeref:typename:const char *
value	./readtags.h	117;"	member	struct:__anon0e8d8a030808	typeref:typename:const char *


findNext	readtags.c	1242;"	f	language:C	typeref:typename:tagResult	file:	signature:(tagFile * const file,tagEntry * const entry)	extras:fileScope
findNext	./readtags.c	1242;"	function	typeref:typename:tagResult	file:	signature:(tagFile * const file,tagEntry * const entry)








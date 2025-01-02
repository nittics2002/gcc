#

## install

```bash
make
make install
```
## uninstall

```bash
make uninstall
```
## setup

1. Geany起動
1. 編集-設定表示
1. 全般 プラグインのパスを下記に設定
1. 再起動
1. ツール-プラグインの設定で HelloWorldを有効化
1. HelloWorldメニューが追加される

## i18n

_()関数で翻訳処理追加
Makefile CFLAGS に -DLOCALEDIR=\"\" -DGETTEXT_PACKAGE=\"foobar\" 追加

>
> -DLOCALEDIR=\"\":
>    LOCALEDIR マクロは、翻訳ファイル（.mo ファイル）が格納されているディレクトリのパスを指定するために使用されます。
>    \"\" として空の文字列を指定している場合、翻訳ファイルのディレクトリが指定されていないことを意味します。通常は、実行時に bindtextdomain 関数を使ってこのディレクトリを設定します。
>
> -DGETTEXT_PACKAGE=\"foobar\":
>    GETTEXT_PACKAGE マクロは、使用する翻訳ファイルの名前を指定します。この名前は、gettext が翻訳ファイルを検索する際に使用されます。
>    \"foobar\" として指定されている場合、gettext は foobar という名前の翻訳ファイル（foobar.mo）を探します。
>



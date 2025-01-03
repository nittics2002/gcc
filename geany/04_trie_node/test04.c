#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 64
#define ASCII_OFFSET 48
#define INITIAL_RESULT_SIZE 10 // 結果の初期サイズ

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int is_end_of_word;
} TrieNode;

typedef struct Trie {
    TrieNode *root;
} Trie;

TrieNode* createNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    node->is_end_of_word = 0;
    return node;
}

Trie* createTrie() {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    trie->root = createNode();
    return trie;
}

int charToIndex(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    } else if (c == '_') {
        return 62;
    } else if (c == '$') {
        return 63;
    }
    return -1; // 無効な文字
}

void insert(Trie *trie, const char *word) {
    TrieNode *current = trie->root;
    while (*word) {
        int index = charToIndex(*word);
        if (index == -1) {
            return; // 無効な文字が含まれている場合は挿入しない
        }
        if (current->children[index] == NULL) {
            current->children[index] = createNode();
        }
        current = current->children[index];
        word++;
    }
    current->is_end_of_word = 1;
}

void searchPrefix(TrieNode *node, const char *prefix, char **results, int *count, int *size) {
    if (node->is_end_of_word) {
        if (*count >= *size) {
            *size *= 2; // サイズを倍に拡張
            results = (char **)realloc(results, (*size) * sizeof(char*));
        }
        results[*count] = strdup(prefix);
        (*count)++;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            char new_prefix[256]; // 新しい接頭辞を格納
            snprintf(new_prefix, sizeof(new_prefix), "%s%c", prefix, 
                     (char)(i < 26 ? 'A' + i : 
                     (i < 52 ? 'a' + (i - 26) : 
                     (i == 62 ? '_' : '$'))));
            searchPrefix(node->children[i], new_prefix, results, count, size);
        }
    }
}

char** findMatchingWords(Trie *trie, const char *prefix, int *result_count) {
    int size = INITIAL_RESULT_SIZE;
    char **results = (char **)malloc(size * sizeof(char*)); // 結果を格納する配列
    *result_count = 0;
    TrieNode *current = trie->root;

    while (*prefix) {
        int index = charToIndex(*prefix);
        if (index == -1) {
            printf("無効な文字が含まれています: %c\n", *prefix);
            free(results);
            return NULL; // 一致する接頭辞がない場合
        }
        current = current->children[index];
        if (current == NULL) {
            return results; // 一致する接頭辞がない場合
        }
        prefix++;
    }

    searchPrefix(current, prefix, results, result_count, &size);
    return results;
}

void freeTrie(TrieNode *node) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            freeTrie(node->children[i]);
        }
    }
    free(node);
}

int main() {
    Trie *trie = createTrie();
    
    // 文字列をトライに追加
    insert(trie, "foo");
    insert(trie, "bar");
    insert(trie, "baz");
    insert(trie, "foobar");
    insert(trie, "fizz_buzz");
    insert(trie, "hello$world");

    // 一致する文字列を検索
    char prefix[100];
    printf("検索する接頭辞を入力してください: ");
    scanf("%99s", prefix); // バッファオーバーフローを防ぐ

    int result_count;
    char **results = findMatchingWords(trie, prefix, &result_count);
    
    if (results != NULL) {
        printf("一致する文字列:\n");
        for (int i = 0; i < result_count; i++) {
            printf("%s\n", results[i]);
            free(results[i]); // 結果のメモリを解放
        }
        free(results); // 結果配列のメモリを解放
    } else {
        printf("一致する単語が見つかりませんでした。\n");
    }

    // メモリを解放
    freeTrie(trie->root);
    free(trie);

    return 0;
}

/**
見直し内容

1. メモリリークの可能性

findMatchingWords関数で、results配列を初期化する際に、固定サイズ（100）でメモリを確保していますが、結果の数が100を超える場合、メモリが不足する可能性があります。動的にメモリを拡張するか、結果の数を制限する必要があります。
2. strdupの使用

strdupは、メモリを動的に確保して文字列を複製しますが、#include <string.h>が必要です。すでにインクルードされていますが、strdupを使用する際は、メモリを解放することを忘れないようにしてください。
3. snprintfのバッファオーバーフロー

snprintfを使用して新しい接頭辞を作成していますが、new_prefixのサイズが256であるため、接頭辞が256文字を超えるとバッファオーバーフローが発生する可能性があります。接頭辞の長さを考慮して、適切なサイズを確保する必要があります。
4. 無効な文字の処理

charToIndex関数で無効な文字が渡された場合、単に-1を返していますが、呼び出し元でのエラーハンドリングが不足しています。無効な文字が含まれている場合は、エラーメッセージを表示するなどの処理を追加すると良いでしょう。
5. main関数の入力処理

scanfを使用して接頭辞を取得していますが、バッファオーバーフローを防ぐために、入力サイズを制限する必要があります。例えば、scanf("%99s", prefix);のようにすることで、最大99文字までの入力を受け付けることができます。
*/

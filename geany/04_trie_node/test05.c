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

void searchPrefix(TrieNode *node, const char *prefix, char ***results, int *count, int *size) {
    if (node->is_end_of_word) {
        if (*count >= *size) {
            *size *= 2; // サイズを倍に拡張
            *results = (char **)realloc(*results, (*size) * sizeof(char*));
            if (*results == NULL) {
                fprintf(stderr, "メモリ割り当てに失敗しました。\n");
                exit(EXIT_FAILURE);
            }
        }
        (*results)[*count] = strdup(prefix);
        if ((*results)[*count] == NULL) {
            fprintf(stderr, "メモリ割り当てに失敗しました。\n");
            exit(EXIT_FAILURE);
        }
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
    char **results = (char **)malloc(size * sizeof(char*));
    if (results == NULL) {
        fprintf(stderr, "メモリ割り当てに失敗しました。\n");
        exit(EXIT_FAILURE);
    }
    *result_count = 0;
    TrieNode *current = trie->root;

    while (*prefix) {
        int index = charToIndex(*prefix);
        if (index == -1) {
            printf("無効な文字が含まれています: %c\n", *prefix);
            free(results);
            return NULL;
        }
        current = current->children[index];
        if (current == NULL) {
            return results; // 一致する接頭辞がない場合
        }
        prefix++;
    }

    searchPrefix(current, prefix, &results, result_count, &size);
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
改善点

メモリ管理の改善:
    searchPrefix関数内でresultsを再割り当てしていますが、ポインタの変更が呼び出し元に反映されません。resultsをポインタのポインタとして渡す必要があります。
    strdupを使用してメモリを動的に割り当てていますが、strdupが失敗した場合のエラーチェックがありません。

エラーハンドリング:
    mallocやreallocの戻り値をチェックして、メモリ割り当てに失敗した場合の処理を追加することをお勧めします。

接頭辞の生成:
    snprintfを使用して新しい接頭辞を生成していますが、new_prefixのサイズを固定で256にしています。接頭辞が長くなる可能性があるため、動的にメモリを割り当てることを検討してください。

定数の使用:
    ALPHABET_SIZEやINITIAL_RESULT_SIZEなどの定数を使用していますが、これらの値を変更する場合、プログラム全体に影響を与える可能性があります。これらの値を設定するための関数や構造体を作成することを検討してください。

無効な文字の処理:
    insert関数で無効な文字が含まれている場合、何もせずに戻るのではなく、エラーメッセージを表示することを検討してください。
*/

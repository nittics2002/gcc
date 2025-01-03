#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 64 // 英字（大文字・小文字） + 数字 + アンダースコア + ドル記号
#define ASCII_OFFSET 48  // '0'のASCIIコード

// トライノードの構造体
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE]; // 子ノードへのポインタ
    int is_end_of_word; // 単語の終端を示すフラグ
} TrieNode;

// トライの構造体
typedef struct Trie {
    TrieNode *root;
} Trie;

// トライノードを作成する関数
TrieNode* createNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    node->is_end_of_word = 0;
    return node;
}

// トライを作成する関数
Trie* createTrie() {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    trie->root = createNode();
    return trie;
}

// 文字をALPHABET_SIZEに基づいてインデックスに変換する関数
int charToIndex(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A'; // 大文字
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26; // 小文字
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52; // 数字
    } else if (c == '_') {
        return 62; // アンダースコア
    } else if (c == '$') {
        return 63; // ドル記号
    }
    return -1; // 無効な文字
}

// トライに単語を挿入する関数
void insert(Trie *trie, const char *word) {
    TrieNode *current = trie->root;
    while (*word) {
        int index = charToIndex(*word);
        if (index == -1) {
            // 無効な文字が含まれている場合は挿入しない
            return;
        }
        if (current->children[index] == NULL) {
            current->children[index] = createNode();
        }
        current = current->children[index];
        word++;
    }
    current->is_end_of_word = 1;
}

// 前方一致する単語を検索する関数
void searchPrefix(TrieNode *node, const char *prefix, char **results, int *count) {
    if (node->is_end_of_word) {
        results[*count] = strdup(prefix); // 結果を複製
        (*count)++;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            char new_prefix[256]; // 新しい接頭辞を格納
            snprintf(new_prefix, sizeof(new_prefix), "%s%c", prefix, (char)(i < 26 ? 'A' + i : (i < 52 ? 'a' + (i - 26) : (i == 62 ? '_' : '$'))));
            searchPrefix(node->children[i], new_prefix, results, count);
        }
    }
}

// 指定された接頭辞に一致する単語を検索する関数
char** findMatchingWords(Trie *trie, const char *prefix, int *result_count) {
    char **results = (char **)malloc(100 * sizeof(char*)); // 結果を格納する配列
    *result_count = 0;
    TrieNode *current = trie->root;

    // 接頭辞の各文字をトライで辿る
    while (*prefix) {
        int index = charToIndex(*prefix);
        if (index == -1) {
            return results; // 一致する接頭辞がない場合
        }
        current = current->children[index];
        if (current == NULL) {
            return results; // 一致する接頭辞がない場合
        }
        prefix++;
    }

    // 接頭辞に続く単語を検索
    searchPrefix(current, prefix, results, result_count);
    return results;
}

// メモリを解放する関数
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
    scanf("%s", prefix);
    
    int result_count;
    char **results = findMatchingWords(trie, prefix, &result_count);
    printf("一致する文字列:\n");
    for (int i = 0; i < result_count; i++) {
        printf("%s\n", results[i]);
        free(results[i]); // 結果のメモリを解放
    }

    // メモリを解放
    free(results);
    freeTrie(trie->root);
    free(trie);

    return 0;
}

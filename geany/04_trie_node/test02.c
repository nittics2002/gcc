#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 256 // ASCII文字のサイズ

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

// トライに単語を挿入する関数
void insert(Trie *trie, const char *word) {
    TrieNode *current = trie->root;
    while (*word) {
        if (current->children[(unsigned char)*word] == NULL) {
            current->children[(unsigned char)*word] = createNode();
        }
        current = current->children[(unsigned char)*word];
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
            snprintf(new_prefix, sizeof(new_prefix), "%s%c", prefix, (char)i);
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
        if (current->children[(unsigned char)*prefix] == NULL) {
            return results; // 一致する接頭辞がない場合
        }
        current = current->children[(unsigned char)*prefix];
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
    insert(trie, "apple");
    insert(trie, "apricot");
    insert(trie, "banana");
    insert(trie, "grape");
    insert(trie, "appliance");

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

typedef struct TrieNode {
    struct TrieNode *children[256]; // ASCII文字用の配列
    gboolean is_end_of_word; // 単語の終端を示すフラグ
} TrieNode;

typedef struct Trie {
    TrieNode *root;
} Trie;

// トライノードを作成する関数
TrieNode* createNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    for (int i = 0; i < 256; i++) {
        node->children[i] = NULL;
    }
    node->is_end_of_word = FALSE;
    return node;
}

// トライを作成する関数
Trie* createTrie() {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    trie->root = createNode();
    return trie;
}

// トライに単語を挿入する関数
void insert(Trie *trie, const gchar *word) {
    TrieNode *current = trie->root;
    while (*word) {
        if (current->children[(unsigned char)*word] == NULL) {
            current->children[(unsigned char)*word] = createNode();
        }
        current = current->children[(unsigned char)*word];
        word++;
    }
    current->is_end_of_word = TRUE;
}

// 前方一致する単語を検索する関数
void searchPrefix(TrieNode *node, const gchar *prefix, GArray *results) {
    if (node->is_end_of_word) {
        g_array_append_val(results, g_strdup(prefix));
    }
    for (int i = 0; i < 256; i++) {
        if (node->children[i] != NULL) {
            gchar new_prefix[256];
            snprintf(new_prefix, sizeof(new_prefix), "%s%c", prefix, (char)i);
            searchPrefix(node->children[i], new_prefix, results);
        }
    }
}

// 指定された接頭辞に一致する単語を検索する関数
GArray* findMatchingWords(Trie *trie, const gchar *prefix) {
    GArray *results = g_array_new(FALSE, FALSE, sizeof(gchar*));
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
    searchPrefix(current, prefix, results);
    return results;
}

// メモリを解放する関数
void freeTrie(TrieNode *node) {
    for (int i = 0; i < 256; i++) {
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
    gchar prefix[100];
    printf("検索する接頭辞を入力してください: ");
    scanf("%s", prefix);
    
    GArray *results = findMatchingWords(trie, prefix);
    printf("一致する文字列:\n");
    for (guint i = 0; i < results->len; i++) {
        gchar *str = g_array_index(results, gchar*, i);
        printf("%s\n", str);
        g_free(str); // 結果のメモリを解放
    }

    // メモリを解放
    g_array_free(results, TRUE);
    freeTrie(trie->root);
    free(trie);

    return 0;
}

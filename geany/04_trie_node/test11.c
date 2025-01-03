#include <stdio.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26

// Trieノードの構造体定義
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord;
} TrieNode;

// 新しいTrieノードを作成する関数
TrieNode *getNode(void) {
    TrieNode *pNode = NULL;
    pNode = (TrieNode *)malloc(sizeof(TrieNode));

    if (pNode) {
        int i;
        pNode->isEndOfWord = 0;
        for (i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
    }

    return pNode;
}

// トライアル木に新しいキーを挿入する関数
void insert(TrieNode *root, const char *key) {
    int level;
    int length = strlen(key);
    int index;
    TrieNode *pCrawl = root;

    for (level = 0; level < length; level++) {
        index = key[level] - 'a';
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
        pCrawl = pCrawl->children[index];
    }

    // 最後のノードを単語の終端としてマーク
    pCrawl->isEndOfWord = 1;
}

// キーがトライアル木に存在するかを検索する関数
int search(TrieNode *root, const char *key) {
    int level;
    int length = strlen(key);
    int index;
    TrieNode *pCrawl = root;

    for (level = 0; level < length; level++) {
        index = key[level] - 'a';
        if (!pCrawl->children[index])
            return 0;
        pCrawl = pCrawl->children[index];
    }

    return (pCrawl != NULL && pCrawl->isEndOfWord);
}

// 与えられたノードから全ての単語を収集するヘルパー関数
void collectWords(TrieNode *root, char *prefix, int length) {
    if (root->isEndOfWord) {
        prefix[length] = '\0';
        printf("%s\n", prefix);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            prefix[length] = 'a' + i;
            collectWords(root->children[i], prefix, length + 1);
        }
    }
}

// 接頭辞に一致する全ての単語を検索する関数
void searchPrefix(TrieNode *root, const char *prefix) {
    int level;
    int length = strlen(prefix);
    int index;
    TrieNode *pCrawl = root;
    char buffer[100];

    for (level = 0; level < length; level++) {
        index = prefix[level] - 'a';
        if (!pCrawl->children[index])
            return;
        pCrawl = pCrawl->children[index];
    }

    strncpy(buffer, prefix, length);
    collectWords(pCrawl, buffer, length);
}


int main() {
    // 根ノードの作成
    TrieNode *root = getNode();

    // キーの挿入
    insert(root, "hello");
    insert(root, "world");

    // キーの検索
    printf("%s --- %s\n", "hello", search(root, "hello") ? "Found" : "Not Found");
    printf("%s --- %s\n", "world", search(root, "world") ? "Found" : "Not Found");

    // キーの削除
    delete(root, "hello", 0);
    printf("%s --- %s\n", "hello", search(root, "hello") ? "Found" : "Not Found");



    // キーの挿入
    insert(root, "hello");
    insert(root, "hell");
    insert(root, "heaven");
    insert(root, "heavy");

    // キーの検索
    printf("%s --- %s\n", "hello", search(root, "hello") ? "Found" : "Not Found");
    printf("%s --- %s\n", "heaven", search(root, "heaven") ? "Found" : "Not Found");
    printf("%s --- %s\n", "hell", search(root, "hell") ? "Found" : "Not Found");
    printf("%s --- %s\n", "goodbye", search(root, "goodbye") ? "Found" : "Not Found");

    // 接頭辞検索
    printf("Words with prefix 'he':\n");
    searchPrefix(root, "he");


    return 0;
}







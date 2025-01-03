//この実装では、ノードが持つ子ノードの数を numChildren で管理し、不要なメモリ割り当てを減らしています。
//しかし、何の意味が?

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord;
    int numChildren;
} TrieNode;

// 新しいTrieノードを作成する関数
TrieNode *getNode(void) {
    TrieNode *pNode = (TrieNode *)malloc(sizeof(TrieNode));
    pNode->isEndOfWord = 0;
    pNode->numChildren = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;
    return pNode;
}

// 子ノードの追加関数
void addChild(TrieNode *node, int index) {
    if (!node->children[index]) {
        node->children[index] = getNode();
        node->numChildren++;
    }
}

// トライアル木に新しいキーを挿入する関数
void insert(TrieNode *root, const char *key) {
    int length = strlen(key);
    TrieNode *pCrawl = root;

    for (int level = 0; level < length; level++) {
        int index = key[level] - 'a';
        addChild(pCrawl, index);
        pCrawl = pCrawl->children[index];
    }

    pCrawl->isEndOfWord = 1;
}

// メモリ解放の関数
void freeTrie(TrieNode *root) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            freeTrie(root->children[i]);
        }
    }
    free(root);
}

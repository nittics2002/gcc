#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

int main() {
    // GArrayの作成
    GArray *str_array = g_array_new(FALSE, FALSE, sizeof(gchar *));
    if (str_array == NULL) {
        fprintf(stderr, "Failed to create GArray\n");
        return EXIT_FAILURE;
    }

    // 文字列の入力
    gint num_strings;
    printf("Enter the number of strings: ");
    if (scanf("%d", &num_strings) != 1 || num_strings < 0) {
        fprintf(stderr, "Invalid number of strings\n");
        g_array_free(str_array, TRUE);
        return EXIT_FAILURE;
    }

    for (gint i = 0; i < num_strings; i++) {
        gchar *str = NULL;
        printf("Enter string %d: ", i + 1);
        if (scanf(" %ms", &str) != 1 || str == NULL) {
            fprintf(stderr, "Failed to read string\n");
            // 既に追加した文字列を解放
            for (gint j = 0; j < str_array->len; j++) {
                g_free(g_array_index(str_array, gchar *, j));
            }
            g_array_free(str_array, TRUE);
            return EXIT_FAILURE;
        }
        g_array_append_val(str_array, str); // 文字列をGArrayに追加
    }

    // 文字列の表示
    printf("Entered strings:\n");
    for (gint i = 0; i < str_array->len; i++) {
        printf("%s\n", g_array_index(str_array, gchar *, i));
    }

    // メモリの解放
    for (gint i = 0; i < str_array->len; i++) {
        g_free(g_array_index(str_array, gchar *, i));
    }
    g_array_free(str_array, TRUE);

    return EXIT_SUCCESS;
}

/**
 * ctags_snippet.c
 *
 * @version 
 */
#include <geanyplugin.h>
#include <stdio.h>

#include <SciLexer.h>

/**
 * @brief 辞書メモリ初期サイズ
 */
const gint8 DICT_INI_LEN = 100;

/**
 * @brief geany plugin
 */
static GeanyPlugin *geany_plugin = NULL;

/**
 * @brief geany data
 */
static GeanyData *geany_data = NULL;

/**
 * @brief メニュー read tags
 */
static GtkWidget *menu_item_read_tags;

/**
 * @brief メニュー view snippets
 */
static GtkWidget *menu_item_view_snippets;

/**
 * @brief メニューキーバインド
 */
enum{
   KB_READ_TAGS,     //ID=0
   KB_VIEW_SNIPPETS, //ID=1
   KB_BIND_COUNT  //count=2
};

/**
 * @brief snippet辞書
 */
static gchar **snippet_dict;

/**
 * @brief snippet辞書サイズ
 */
static gint snippet_dict_length = 0;

/**
 * @brief 辞書メモリエリア数
 */
static gint dict_count = 1;

/**
 * @brief 辞書インデックス
 *      idx=0:A, 1:B, ...,25:Z,26:その他
 *      val=辞書idx(1〜) 0:登録なし
 */
static gchar dict_index[27];

/**
 * @brief snippetリスト
 */
static gchar **snippet_list;

/**
 * @brief snippetリストサイズ
 */
static gint snippet_list_length = 0;







/**
 * @brief get_tags_filename
 * @return *gchar
 */
static gchar *get_tags_filename(void)
{
    gchar *ret = NULL;

    if (geany_data->app->project) {
	ret = utils_remove_ext_from_filename(geany_data->app->project->file_name);

	SETPTR(ret, g_strconcat(ret, ".tags", NULL));
    }

    return ret;
}

/**
 * @brief on_read_tagfile
 * @param GtkMenuItem *menuitem
 * @param gpointer user_data
 * @return gboolean
 */
static gboolean on_read_tagfile(
    GtkMenuItem *menuitem,
    gpointer user_data
) {
    gchar *tags_file_path;

    tags_file_path = get_tags_filename();

    if (! g_file_test(tags_file_path, G_FILE_TEST_EXISTS)) {
        dialogs_show_msgbox(
	    GTK_MESSAGE_ERROR,
	    "You need to create a ctags database with Geanyctags"
	);
        return FALSE;
    }

    FILE *fp;
    fp = fopen(tags_file_path, "r");

    if (fp == NULL) {
	dialogs_show_msgbox(
	    GTK_MESSAGE_ERROR,
	    "tags file open error"
	);
	return FALSE;
    }

    gchar buffer[512];
    gchar *row_string;
    gchar **splited;
    gchar *tag_name6;
    gchar *tag_name1;
    gchar tag_name1_c;
    gchar *prev_tag_name;
    gchar prev_tag_name1_c;
    gchar count = 0;
    gchar idx;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        row_string = g_strdup(buffer);
       
        //タグ名抽出 
        splited = g_strsplit(row_string, "\t", 3);
        
        tag_name6 = g_strndup(splited[0], 6);

        //重複タグ名・ファイル情報等除外
        if (
            g_strcmp0(tag_name6, "!_TAG_") != 0 &&
            g_strcmp0(tag_name6, "__anon") != 0 &&
            g_strcmp0(splited[0], prev_tag_name) != 0
       ) {
            //辞書サイズ再確保
            if (
            count % DICT_INI_LEN == 0 &&
            count > 0
           ){
                snippet_dict = (gchar**)realloc(
		    snippet_dict,
		    DICT_INI_LEN *
		    dict_count *
		    sizeof(gchar *)
		);
		
		if (snippet_dict == NULL) {
		    msgwin_switch_tab(MSG_STATUS, TRUE);
		    msgwin_status_add("Faild to reallocate memory");
		    return FALSE;
		}

		dict_count++;
	    }
            //先頭文字を抽出し大文字小文字無視でindex
            tag_name1 = g_strndup(splited[0], 1);
            tag_name1_c = g_ascii_toupper(tag_name1[0]);

            //先頭文字が変わった辞書index+1を記憶
            if (prev_tag_name1_c != tag_name1_c) {
                idx = tag_name1_c - 65;
                idx = idx < 0 || idx > 25? 26:idx;

                dict_index[idx] = count + 1; 
            }

            snippet_dict[count] = g_strdup(splited[0]);
	    
            prev_tag_name1_c = tag_name1_c;
            
//msgwin_status_add("%d=%s",count, snippet_dict[count]); 
            
            count++;
        }

	prev_tag_name = g_strdup(splited[0]);
    }    
    
    count;
    snippet_dict_length = count;

/*

msgwin_status_add("xxxxxxxxxxxxxxxxx"); 
msgwin_status_add("size=%d",snippet_dict_length); 

*/


    fclose(fp);
    g_free(prev_tag_name);
    g_free(tag_name1);
    g_free(tag_name6);
    g_strfreev(splited);
    g_free(row_string);
    g_free(tags_file_path);

    //msgwin_switch_tab(MSG_STATUS, TRUE);
    msgwin_status_add("Ctags Snippet loaded");

    return TRUE;
}

/**
 * @brief is_delimiter
 * @param gchar c
 * @return gboolean
 */
static gboolean is_delimiter(
    gchar c
)
{
    gint i;

    for(i = 32; i <= 47; i++) {
        if (c == (gchar)i) {
            return TRUE;
        }
    }

    for(i = 58; i <= 64; i++) {
        if (c == (gchar)i) {
            return TRUE;
        }
    }
    
    //_を除外
    for(i = 91; i <= 94; i++) {
        if (c == (gchar)i) {
            return TRUE;
        }
    }

    if (c == 96) return TRUE;
    
    for(i = 123; i <= 127; i++) {
        if (c == (gchar)i) {
            return TRUE;
        }
    }
    
    for(i = 0; i <= 31; i++) {
        if (c == i) {
            return TRUE;
        }
    }
    
    return FALSE;
}






static gchar* prefix(
    gchar* fragment
){
    gchar *first_char;
    gchar first_code;
    gchar index;
    gint snippet_length;
    gint dict_start;
    gint dict_end;
    gint list_length;
    gint i;

//msgwin_status_add("-------------=");

    first_char = g_strndup(fragment, 1);

    first_code = g_ascii_toupper(first_char[0]);

    index = first_code - 65;

    if (index < 0 || index > 25) index = 26;
    
    dict_start = dict_index[index];
    
    //先頭文字マッチなし
    if (dict_start == 0) {
        return NULL;
    }

    //A-Z以外で始まる場合
    if (index == 26) {
        dict_end = snippet_dict_length + 1;
    } else {
        for (i = index + 1; i <= 26; i++) {
            if (dict_index[i] != 0){
                dict_end = dict_index[i];
                break;
            }
        }
    }

    if (!dict_end) {
        dict_end = snippet_dict_length + 1;
    }

    //snippet_list配列確保
    snippet_list_length = 0;

    list_length = dict_end - dict_start;

    list_length = list_length < 0?
        0:list_length;

    if (snippet_list != 0) {
        g_free(snippet_list);


        msgwin_status_add("Fee snnipet_list");


    }



    snippet_list = (gchar **)malloc(
        list_length *
        sizeof(gchar *)
    );

    if (snippet_dict == NULL) {
        msgwin_switch_tab(MSG_STATUS, TRUE);
        msgwin_status_add("Faild to allocate memory");
        return FALSE;
    }

    for (i = dict_start - 1; i < dict_end - 1; i++) {
        if (g_str_has_prefix(snippet_dict[i], fragment)) {
            snippet_list[i] = snippet_dict[i]; 
            snippet_list_length++;
            
            
            //msgwin_status_add("%s", snippet_dict[i]);
        }
    }


msgwin_status_add("snippet_list_length=%d", snippet_list_length);



    g_free(first_char);
    
return snippet_dict[0];


}

/**
 * @brief on_view_snippet
 * @param GtkMenuItem *menuitem
 * @param gpointer user_data
 * @return gboolean
 */
static gboolean on_view_snippet(
    GtkMenuItem *menuitem,
    gpointer user_data
) {
    if (snippet_dict == NULL) {
	dialogs_show_msgbox(
	    GTK_MESSAGE_ERROR,
	    "tags file not loaded"
	);
	return FALSE;
    }

    ScintillaObject *sci;
    gint pos;
    gchar chr;
    gchar buffer[512];
    gint i = 0;
    gint j, k;
    //gchar *snippets;

    sci = document_get_current()
        ->editor
        ->sci;

    pos = sci_get_current_position(sci);

    if (pos == 0) {
        return TRUE;
    }
    
    pos--;

    while(pos >= 0){
        chr = sci_get_char_at(sci, pos);

        if (is_delimiter(chr)) {
            gchar fragment[i];

            k = i - 1;

            for(j = 0; j < i; j++) {
                fragment[j] = buffer[k];
                k--;
            }

            fragment[j] = '\0';

            prefix(fragment) ;










//msgwin_status_add("fragment=%s",fragment);


            break;
        }
        
        buffer[i] = chr;
        i++;
        pos--;
    }

    return TRUE;
}

/**
 * @brief kb_read_tags
 * @param G_GNUC_UNUSED guint key_id
 * @return void
 */
static void kb_read_tags(
    G_GNUC_UNUSED guint key_id
){
   /* sanity check */
   if (document_get_current() == NULL){
	return;
   }
   on_read_tagfile(NULL, NULL);
}

/**
 * @brief kb_view_snippets
 * @param G_GNUC_UNUSED guint key_id
 * @return void
 */
static void kb_view_snippets(
    G_GNUC_UNUSED guint key_id
){
   /* sanity check */
   if (document_get_current() == NULL){
	return;
   }
   on_view_snippet(NULL, NULL);
}

/**
 * @brief add_menu
 * @return void
 */
static void add_menu()
{
    //キー割り当てグループ
    GeanyKeyGroup *key_group;

    key_group = plugin_set_key_group(
	geany_plugin,
	"CtagsSnippet",
	KB_BIND_COUNT,
	NULL
    );

    //メニューread ctags
    menu_item_read_tags = gtk_menu_item_new_with_mnemonic(
        "Ctags Snippet read tags"
    );
    
    gtk_widget_show(menu_item_read_tags);

    gtk_container_add(
	GTK_CONTAINER(geany_data->main_widgets->project_menu),
	menu_item_read_tags
    );
            
    g_signal_connect(
	menu_item_read_tags,
	"activate",
	G_CALLBACK(on_read_tagfile),
	NULL
    );
 
    geany_plugin_set_data(geany_plugin, menu_item_read_tags, NULL);

   /* make sure our menu items aren't called when there is no doc open */
   //ui_add_document_sensitive(menu_item_read_tags);
//   ui_add_document_sensitive(menu_item_shift_left);

    keybindings_set_item(
	key_group,
	KB_READ_TAGS,
	kb_read_tags,
	0,
	0,
	"read_tags",
	"Read Tags",
	menu_item_read_tags
    );

    //メニュー view snippets
    menu_item_view_snippets = gtk_menu_item_new_with_mnemonic(
        "Ctags Snippet view snippets"
    );
    
    gtk_widget_show(menu_item_view_snippets);

    gtk_container_add(
	GTK_CONTAINER(geany_data->main_widgets->project_menu),
	menu_item_view_snippets
    );
            
    g_signal_connect(
	menu_item_view_snippets,
	"activate",
	G_CALLBACK(on_view_snippet),
	NULL
    );
 
    geany_plugin_set_data(geany_plugin, menu_item_view_snippets, NULL);

    keybindings_set_item(
	key_group,
	KB_VIEW_SNIPPETS,
	kb_view_snippets,
	0,
	0,
	"view_snippets",
	"View Snippets",
	menu_item_view_snippets
    );

    msgwin_switch_tab(MSG_STATUS, TRUE);
    msgwin_status_add("Activate Ctags Snippet");

}

/**
 * @brief ctags_snippet_init
 * @param GeanyPlugin *plugin
 * @param gpointer pdata
 * @return gboolean
 */
static gboolean ctags_snippet_init(
    GeanyPlugin *plugin,
    gpointer pdata
){
    geany_plugin = plugin;
    geany_data = plugin->geany_data;

    //snippet配列確保
    snippet_dict = (gchar **)malloc(
	    DICT_INI_LEN *
        dict_count *
        sizeof(gchar *)
    );

    dict_count++;

    if (snippet_dict == NULL) {
        msgwin_switch_tab(MSG_STATUS, TRUE);
        msgwin_status_add("Faild to allocate memory");
        return FALSE;
    }

    //メニュー表示設定
    add_menu();
    
    return TRUE;
}

/**
 * @brief free_memmory
 * @return void
 */
static void free_memmory()
{
    //g_free(dict_count);
    g_free(snippet_list);
    g_free(snippet_dict);
}

/**
 * @brief ctags_snippet_cleanup
 `b\\* @param GeanyPlugin *plugin
 * @param gpointer pdata
 * @return void
 */
static void ctags_snippet_cleanup(
    GeanyPlugin *plugin,
    gpointer pdata
){
    free_memmory();

    gtk_widget_destroy(menu_item_read_tags);
    gtk_widget_destroy(menu_item_view_snippets);

    msgwin_switch_tab(MSG_STATUS, TRUE);
    msgwin_status_add("Deactive Ctags Snippet");
}

/**
 * @brief geany_load_module
 * @param GeanyPlugin *plugin
 * @return void
 */
G_MODULE_EXPORT
void geany_load_module(
    GeanyPlugin *plugin
){
    plugin->info->name = "Ctags Snippet";
    plugin->info->description = "Type-ahead with U-ctags";
    plugin->info->version = "0.1";
    plugin->info->author = "foobar";

    plugin->funcs->init = ctags_snippet_init;
    plugin->funcs->cleanup = ctags_snippet_cleanup;

    GEANY_PLUGIN_REGISTER(plugin, 248);
}

/**
 * ctags_snippet.c
 *
 * @version 
 */
#include <geanyplugin.h>
#include <stdio.h>

#include <SciLexer.h>

/**
 * @const gint8 DICT_INI_LEN
 */
const gint8 DICT_INI_LEN = 2;

/**
 * @var *gchar geany_plugin
 */
static GeanyPlugin *geany_plugin = NULL;

/**
 * @var *gchar geany_data
 */
static GeanyData *geany_data = NULL;

/**
 * @var **gchar snippet_dict
 */
static gchar **snippet_dict;

/**
 * @var gint dict_count
 */
static gint dict_count = 1;




typedef struct InputInfo
{
    gint tag_start;
} InputInfo;

typedef struct CompletionInfo
{
    gchar *completion;
} CompletionInfo;

gboolean get_completion(
    GeanyEditor *editor,
    const gchar *sel,
    const gint size,
    CompletionInfo * c,
    InputInfo * i
) {
    return FALSE;
}




/**
 * @brief ctags_snippet_editor_notify
 * @param GObject *object
 * @param GeanyEditor *editor
 * @param SCNotification *nt
 * @param gpointer data
 * @return gboolean
 */
static gboolean ctags_snippet_editor_notify(
    GObject *object,
    GeanyEditor *editor,
    SCNotification *nt,
    gpointer data
) {
/*

    gint lexer, pos, style, min, size;
    gboolean handled = FALSE;

    if (nt->nmhdr.code == SCN_CHARADDED && nt->ch == '>') {
	pos = sci_get_current_position(editor->sci);
	style = sci_get_style_at(editor->sci, pos);

	if (
	    (style <= SCE_H_XCCOMMENT || highlighting_is_string_style(lexer, style)) &&
	    !highlighting_is_comment_style(lexer, style)
	){
	    CompletionInfo c;
	    InputInfo i;
	    gchar *sel;

	    //Grab the last 512 characters or so
	    min = pos - 512;
	    if (min < 0) min = 0;
	    size = pos - min;

	    sel = sci_get_contents_range(editor->sci, min, pos);

	    if (get_completion(editor, sel, size, &c, &i))
	    {
		//Remove typed opening tag
		sci_set_selection_start(editor->sci, min + i.tag_start);
		sci_set_selection_end(editor->sci, pos);
		sci_replace_sel(editor->sci, "");
		pos -= (size - i.tag_start); //pos has changed while deleting

		//Insert the completion
		editor_insert_snippet(editor, pos, c.completion);
		sci_scroll_caret(editor->sci);

		g_free((gchar *)c.completion);
		handled = TRUE;
	    }

	    g_free(sel);
	}

	//
	msgwin_status_add("called ctags_snippet_editor_notify\n");

	
    }

    //return handled;
*/

return TRUE;
}



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
    gchar *prev_tag_name;
    gchar count = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        row_string = g_strdup(buffer);
        
        splited = g_strsplit(row_string, "\t", 3);
        
        tag_name6 = g_strndup(splited[0], 6);

        if (
            g_strcmp0(tag_name6, "!_TAG_") != 0 &&
            g_strcmp0(tag_name6, "__anon") != 0 &&
	    g_strcmp0(splited[0], prev_tag_name) != 0
       ) {
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
                    msgwin_switch_tab(MSG_MESSAGE, TRUE);
                    msgwin_msg_add(COLOR_RED, -1, NULL, "Faild to reallocate memory");
		    return FALSE;
                }

		dict_count++;
            }

	    snippet_dict[count] = g_strdup(splited[0]);
            
//msgwin_msg_add(COLOR_BLUE, -1, NULL, "%s",splited[0]); 
msgwin_msg_add(COLOR_BLUE, -1, NULL, "%s",snippet_dict[count]); 
            
            count++;
        }

	prev_tag_name = g_strdup(splited[0]);
    }    
    
    fclose(fp);
    g_free(prev_tag_name);
    g_free(tag_name6);
    g_strfreev(splited);
    g_free(row_string);
    g_free(tags_file_path);

    msgwin_switch_tab(MSG_MESSAGE, TRUE);
    msgwin_msg_add(COLOR_BLUE, -1, NULL, "Ctags Snippet loaded");


    return TRUE;
}

/**
 * @brief add_menu
 * @return void
 */
static void add_menu()
{
    GtkWidget *main_menu_item;
    main_menu_item = gtk_menu_item_new_with_mnemonic(
        "Ctags Snippet read tags"
    );
    
    gtk_widget_show(main_menu_item);

    gtk_container_add(
	GTK_CONTAINER(geany_data->main_widgets->project_menu),
	main_menu_item
    );
            
    g_signal_connect(
	main_menu_item,
	"activate",
	G_CALLBACK(on_read_tagfile),
	NULL
    );
 
    geany_plugin_set_data(geany_plugin, main_menu_item, NULL);

    msgwin_switch_tab(MSG_MESSAGE, TRUE);
    msgwin_msg_add(COLOR_BLUE, -1, NULL, "Activate Ctags Snippet");

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

    snippet_dict = (gchar **)malloc(
	    DICT_INI_LEN *
        dict_count *
        sizeof(gchar *)
    );

    dict_count++;

    if (snippet_dict == NULL) {
	msgwin_switch_tab(MSG_MESSAGE, TRUE);
	msgwin_msg_add(COLOR_RED, -1, NULL, "Faild to allocate memory");
    return FALSE;
    }

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
    g_free(snippet_dict);
}

/**
 * @brief ctags_snippet_cleanup
 * @param GeanyPlugin *plugin
 * @param gpointer pdata
 * @return void
 */
static void ctags_snippet_cleanup(
    GeanyPlugin *plugin,
    gpointer pdata
){
    free_memmory();

    GtkWidget *main_menu_item = (GtkWidget *) pdata;
 
    gtk_widget_destroy(main_menu_item);

    msgwin_switch_tab(MSG_MESSAGE, TRUE);
    msgwin_msg_add(COLOR_BLUE, -1, NULL, "Deactive Ctags Snippet");
}

/**
 * @brief ctags_snippet_callbacks
 * @var PluginCallback ctags_snippet_callbacks[]
 */
PluginCallback ctags_snippet_callbacks[] =
{
	{ "editor-notify", (GCallback) &ctags_snippet_editor_notify, FALSE, NULL },
	{ NULL, NULL, FALSE, NULL }
};

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
    plugin->funcs->callbacks = ctags_snippet_callbacks;

    GEANY_PLUGIN_REGISTER(plugin, 248);
}

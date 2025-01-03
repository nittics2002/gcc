/**
 * ctags_snippet.c
 *
 * @version 
 */
#include <geanyplugin.h>
#include <SciLexer.h>

/**
 * @var *gchar 
 */
static GeanyPlugin *geany_plugin = NULL;

/**
 * @var *gchar 
 */
static GeanyData *geany_data = NULL;




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
 * @brief ctags_snippet_get_base_path
 * @return void
 */
static gchar *ctags_snippet_get_base_path(void)
{
    gchar *ret;
    GeanyProject *prj = geany_data->app->project;
    gchar *project_dir_utf8;

    if (!prj)
	return NULL;

    if (g_path_is_absolute(prj->base_path))
	    return g_strdup(prj->base_path);

    project_dir_utf8 = g_path_get_dirname(prj->file_name);
    
    ret = g_build_filename(project_dir_utf8, prj->base_path, NULL);
    
    g_free(project_dir_utf8);

    return ret;
}

/**
 * @brief ctags_snippet_get_tags_filename
 * @return *gchar
 */
static gchar *ctags_snippet_get_tags_filename(void)
{
    gchar *ret = NULL;

    if (geany_data->app->project) {
	ret = utils_remove_ext_from_filename(geany_data->app->project->file_name);

	SETPTR(ret, g_strconcat(ret, ".tags", NULL));
    }

    return ret;
}

/**
 * @brief ctags_snippet_on_read_tagfile
 * @param GtkMenuItem *menuitem
 * @param gpointer user_data
 * @return gboolean
 */
static gboolean ctags_snippet_on_read_tagfile(
    GtkMenuItem *menuitem,
    gpointer user_data
) {
    gchar *ctags_file_path;

    ctags_file_path = ctags_snippet_get_tags_filename();

    if (! g_file_test(ctags_file_path, G_FILE_TEST_EXISTS)) {
	dialogs_show_msgbox(
	    GTK_MESSAGE_ERROR,
	    "You need to create a ctags database with Geanyctags"
	);
	return FALSE;
    }









    
    msgwin_switch_tab(MSG_MESSAGE, TRUE);
    msgwin_msg_add(COLOR_BLUE, -1, NULL, "Ctags Snippet loaded");

    return TRUE;
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

    GtkWidget *main_menu_item;
    main_menu_item = gtk_menu_item_new_with_mnemonic("Ctags read");
    
    gtk_widget_show(main_menu_item);

    gtk_container_add(
	GTK_CONTAINER(plugin->geany_data->main_widgets->tools_menu),
	main_menu_item
    );
            
    g_signal_connect(
	main_menu_item,
	"activate",
	G_CALLBACK(ctags_snippet_on_read_tagfile),
	NULL
    );
 
    geany_plugin_set_data(plugin, main_menu_item, NULL);

    msgwin_switch_tab(MSG_MESSAGE, TRUE);
    msgwin_msg_add(COLOR_BLUE, -1, NULL, "Activate Ctags Snippet");
    
    return TRUE;
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

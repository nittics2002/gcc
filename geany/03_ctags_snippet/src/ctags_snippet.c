/**
 * ctags_snippet.c
 *
 * @version 
 */
#include <geanyplugin.h>
#include <SciLexer.h>




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

	    /* Grab the last 512 characters or so */
	    min = pos - 512;
	    if (min < 0) min = 0;
	    size = pos - min;

	    sel = sci_get_contents_range(editor->sci, min, pos);

	    if (get_completion(editor, sel, size, &c, &i))
	    {
		/* Remove typed opening tag */
		sci_set_selection_start(editor->sci, min + i.tag_start);
		sci_set_selection_end(editor->sci, pos);
		sci_replace_sel(editor->sci, "");
		pos -= (size - i.tag_start); /* pos has changed while deleting */

		/* Insert the completion */
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
    return handled;
}
 
/**
 * @brief ctags_snippet_item_activate
 * @param GtkMenuItem *menuitem
 * @param gpointer user_data
 * @return void
 */
static void ctags_snippet_item_activate(
    GtkMenuItem *menuitem,
    gpointer user_data
) {
    dialogs_show_msgbox(GTK_MESSAGE_INFO, "Ctags snippet");
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
    GtkWidget *main_menu_item;
 
    main_menu_item = gtk_menu_item_new_with_mnemonic("Ctags snippet");
    
    gtk_widget_show(main_menu_item);

    gtk_container_add(
	GTK_CONTAINER(plugin->geany_data->main_widgets->tools_menu),
	main_menu_item
    );
            
    g_signal_connect(
	main_menu_item,
	"activate",
	G_CALLBACK(ctags_snippet_item_activate),
	NULL
    );
 
    geany_plugin_set_data(plugin, main_menu_item, NULL);
    
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

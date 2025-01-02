#include "plugin.h"
#include "xmlsnippets.h"
#include <SciLexer.h>

static gboolean editor_notify_cb(GObject *object, GeanyEditor *editor,
								SCNotification *nt, gpointer data)
{
	gint lexer, pos, style, min, size;
	gboolean handled = FALSE;

	if (nt->nmhdr.code == SCN_CHARADDED && nt->ch == '>')
	{
		lexer = sci_get_lexer(editor->sci);
		if (lexer == SCLEX_XML || lexer == SCLEX_HTML)
		{
			pos = sci_get_current_position(editor->sci);
			style = sci_get_style_at(editor->sci, pos);

			if ((style <= SCE_H_XCCOMMENT || highlighting_is_string_style(lexer, style)) &&
				!highlighting_is_comment_style(lexer, style))
			{
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
		}
	}
	return handled;
}
 
static void item_activate_cb(GtkMenuItem *menuitem, gpointer user_data)
{
    dialogs_show_msgbox(GTK_MESSAGE_INFO, "XML snippet");
}
 
static gboolean py_snppet_init(GeanyPlugin *plugin, gpointer pdata)
{
    GtkWidget *main_menu_item;
 
    // Create a new menu item and show it
    main_menu_item = gtk_menu_item_new_with_mnemonic("XML snippet");
    
    gtk_widget_show(main_menu_item);

    gtk_container_add(GTK_CONTAINER(plugin->geany_data->main_widgets->tools_menu),
            main_menu_item);
            
    g_signal_connect(main_menu_item, "activate",
            G_CALLBACK(item_activate_cb), NULL);
 
    geany_plugin_set_data(plugin, main_menu_item, NULL);
    
    return TRUE;
}

PluginCallback py_snppet_callbacks[] =
{
	{ "editor-notify", (GCallback) &editor_notify_cb, FALSE, NULL },
	{ NULL, NULL, FALSE, NULL }
};

static void py_snppet_cleanup(GeanyPlugin *plugin, gpointer pdata)
{
    GtkWidget *main_menu_item = (GtkWidget *) pdata;
 
    gtk_widget_destroy(main_menu_item);
}

G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin)
{
    plugin->info->name = "XML Snippet";
    plugin->info->description = "XML snippet";
    plugin->info->version = "1.0";
    plugin->info->author = "demo";

    plugin->funcs->init = py_snppet_init;
    plugin->funcs->cleanup = py_snppet_cleanup;
    plugin->funcs->callbacks = py_snppet_callbacks;

    GEANY_PLUGIN_REGISTER(plugin, 248);
}

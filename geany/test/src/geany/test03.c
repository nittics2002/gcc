#include <geanyplugin.h>
#include "pluginextension.h"

static gboolean autocomplete_provided(GeanyDocument *doc, gpointer data)
{
    return doc->file_type->id == GEANY_FILETYPES_PYTHON;
}


static void autocomplete_perform(GeanyDocument *doc, gboolean force, gpointer data)
{
    const gchar *kwd_str = "False None True and as assert async await break case class continue def del elif else except finally for from global if import in is lambda match nonlocal not or pass raise return try while with yield";
    gint pos = sci_get_current_position(doc->editor->sci);
    gchar *word = editor_get_word_at_pos(doc->editor, pos, NULL);
    gchar **kwd;

    if (word && *word)
    {
        GString *words = g_string_sized_new(100);
        gchar **kwds = g_strsplit(kwd_str, " ", -1);

        foreach_strv(kwd, kwds)
        {
            if (g_str_has_prefix(*kwd, word))
            {
                if (words->len > 0)
                    g_string_append(words, "\n");

                g_string_append(words, *kwd);
            }
        }

        scintilla_send_message(doc->editor->sci, SCI_AUTOCSHOW, strlen(word), (sptr_t) words->str);
        g_string_free(words, TRUE);
        g_strfreev(kwds);
    }

    g_free(word);
}


static PluginExtension extension = {
    .autocomplete_provided = autocomplete_provided,
    .autocomplete_perform = autocomplete_perform
};


static gboolean on_editor_notify(G_GNUC_UNUSED GObject *obj, GeanyEditor *editor, SCNotification *nt,
    G_GNUC_UNUSED gpointer user_data)
{
    if (nt->nmhdr.code == SCN_AUTOCSELECTION)
    {
        if (plugin_extension_autocomplete_provided(editor->document, &extension))
        {
            /* we can be sure it was us who performed the autocompletion and
             * not Geany or some other plugin extension */
            msgwin_status_add("PluginExtensionDemo autocompleted '%s'", nt->text);
        }
    }

    return FALSE;
}


//////

static void item_activate_cb(GtkMenuItem *menuitem, gpointer user_data)
{
    dialogs_show_msgbox(GTK_MESSAGE_INFO, "XML snippet");
}

static PluginCallback test03_callbacks[] = {
    {"editor-notify", (GCallback) &on_editor_notify, FALSE, NULL},
    {NULL, NULL, FALSE, NULL}
};

static gboolean test03_init(GeanyPlugin *plugin, gpointer pdata)
{
    GtkWidget *main_menu_item;
 
    // Create a new menu item and show it
    main_menu_item = gtk_menu_item_new_with_mnemonic("test03");
    
    gtk_widget_show(main_menu_item);

    gtk_container_add(GTK_CONTAINER(plugin->geany_data->main_widgets->tools_menu),
            main_menu_item);
            
    g_signal_connect(main_menu_item, "activate",
            G_CALLBACK(item_activate_cb), NULL);
 
    geany_plugin_set_data(plugin, main_menu_item, NULL);



    //extension register
    //plugin_extension_register(&extension, "Python keyword autocompletion", 450, NULL);


    //msgwin_status_add("called test03_init\n");

    return TRUE;
}


static void test03_cleanup(GeanyPlugin *plugin, gpointer pdata)
{
    //plugin_extension_unregister(&extension);

    GtkWidget *main_menu_item = (GtkWidget *) pdata;
 
    gtk_widget_destroy(main_menu_item);
}


G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin)
{
    plugin->info->name = "MyPluginExtensionDemo";
    plugin->info->description = "Demo performing simple Python keyword autocompletion";
    plugin->info->version = "1.0";
    plugin->info->author = "John Doe <john.doe@example.org>";

    plugin->funcs->init = test03_init;
    plugin->funcs->cleanup = test03_cleanup;
    plugin->funcs->callbacks = test03_callbacks;

    GEANY_PLUGIN_REGISTER(plugin, 248);
}

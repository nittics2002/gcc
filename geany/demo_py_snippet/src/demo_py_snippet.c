#include <geanyplugin.h>
#include <pluginextension.h>

static gboolean autocomplete_provided(GeanyDocument *doc, gpointer data)
{
    //ctagsファイルで判定
    return TRUE;
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

static PluginCallback plugin_callbacks[] = {
    {"editor-notify", (GCallback) &on_editor_notify, FALSE, NULL},
    {NULL, NULL, FALSE, NULL}
};

static gboolean init_func(GeanyPlugin *plugin, gpointer pdata)
{
    plugin_extension_register(&extension, "Python Dnippet", 150, NULL);
    return TRUE;
}

static void cleanup_func(GeanyPlugin *plugin, gpointer pdata)
{
    plugin_extension_unregister(&extension);
}

G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin)
{
    plugin->info->name = "PluginPythonSnippet";
    plugin->info->description = "python snippet";
    plugin->info->version = "1.0";
    plugin->info->author = "demo";

    plugin->funcs->init = init_func;
    plugin->funcs->cleanup = cleanup_func;
    plugin->funcs->callbacks = plugin_callbacks;

    GEANY_PLUGIN_REGISTER(plugin, 248);
}

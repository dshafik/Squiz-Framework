function GUIViper(id, settings)
{
    var viperOpts = {
        viperURL: settings.baseURL + '/viper',
    };

    var self  = this;
    var viper = new Viper(viperOpts, function(viperObj) {
        self.viper = viperObj;
        //ViperPluginManager.addPluginSet('simple', ['ViperCoreStylesPlugin', 'ViperKeyboardEditorPlugin',  'ViperRedoPlugin', {name: 'ViperCopyPastePlugin', settings: {pasteType: 'raw'}}], true);
        ViperPluginManager.addPluginSet('all', ['ViperToolbarPlugin', 'ViperCoreStylesPlugin', 'ViperKeyboardEditorPlugin',
            'ViperRedoPlugin', 'ViperCopyPastePlugin', 'ViperFormatPlugin', 'ViperListPlugin', 'ViperTrackChangesPlugin'], true);

        // Set plugin settings.
        self.setPluginSettings(settings.pluginSettings);

        if (settings.toolbarParentid) {
            var toolbarParentElem = dfx.getId(settings.toolbarParentid) || document.body;
            viperObj.setPluginSettings('ViperToolbarPlugin', {parent: toolbarParentElem});
        }

        ViperPluginManager.usePluginSet('all');
        viperObj.setEditableElement(dfx.getId(settings.editableElements[0]));
    });
}

GUIViper.prototype = {
    setPluginSettings: function(settings)
    {
        if (!settings) {
            return;
        }

        var self = this;
        dfx.foreach(settings, function(pluginName) {
            ViperPluginManager.setPluginSettings(pluginName, settings[pluginName]);
        });

    }

};

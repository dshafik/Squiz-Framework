function GUIScreenSwitcher(id, settings) {
    this.id       = id;
    this.settings = settings;
    this.elem       = dfx.getId(this.id);
    this.targetid   = this.settings.target;
    this.targetElem = dfx.getId(this.targetid);

    this.currScreen = {
        system: null,
        id: null
    };

    this.includedFiles = {};
    this.loadedWidgets = [];

    /**
     * screenLoadStarts event.
     * screenLoadFinished event.
     */
    GUI.addWidgetEvent(this, 'screenLoadStarts');
    GUI.addWidgetEvent(this, 'screenLoadFinished');
    //this.addScreenLoadStartsCallback(function() {alert('hi');});
    //this.fireScreenLoadStartsCallbacks();

    this.init();
}

GUIScreenSwitcher.prototype = {

    init: function()
    {
        var self = this;
        dfx.foreach(this.settings.screens, function(idx) {
            var screenid = self.id + '-' + self.settings.screens[idx].id;
            dfx.addEvent(dfx.getId(screenid), 'click', function(evt) {
                if (self.currScreen.id !== self.settings.screens[idx].id) {
                    if (self.settings.screens[idx].disabled !== 'true') {
                        self.currScreen = {
                            system: self.settings.screens[idx].system,
                            id: self.settings.screens[idx].id,
                        },
                        self.loadScreen(
                            self.settings.screens[idx].system,
                            self.settings.screens[idx].id
                        );
                    }
                }
            });

            return true;
        });

    },

    loadScreen: function(systemName, screenId)
    {
        if (arguments.length === 0) {
            systemName = this.currScreen.system;
            screenId   = this.currScreen.id;
        }

        var self   = this;
        var params = {
            systemName: systemName,
            screenId:   screenId.substr(0, 1).toUpperCase() + screenId.substr(1) + 'Screen'
        };

        sfapi.get('GUIScreenSwitcher', 'getScreenContents', params, function(data) {
            // Set the contents.
            dfx.setHtml(self.targetElem, data.result.contents);

            // Load the CSS files.
            var headTag = document.getElementsByTagName('head').item(0);

            for (var templateKey in data.result.cssIncludes) {
                var templateCss = data.result.cssIncludes[templateKey];

                for (var i = 0; i < templateCss.length; i++) {
                    var cssFile = templateCss[i];

                    if (!self.includedFiles[cssFile]) {
                        self.includedFiles[cssFile] = true;
                        var newLink = document.createElement('link');
                        newLink.setAttribute('rel', 'stylesheet');
                        newLink.setAttribute('type', 'text/css');
                        newLink.setAttribute('href', cssFile);
                        headTag.appendChild(newLink);
                    }
                }//end for
            }//end for

            // Create a callback function template for loading widgets.
            var callbackFn = function(templateKey) {
                this.execute = function() {
                    if (data.result.widgets[templateKey]) {
                        for (var j = 0; j < data.result.widgets[templateKey].length; j++) {
                            var widget = data.result.widgets[templateKey][j];
                            self.loadedWidgets.push(widget.id);

                            var widgetObj = eval('new ' + widget.type
                                + '(widget.id, widget.settings)');
                            GUI.addWidget(widget.id, widgetObj);
                        }
                    }
                };
            };

            // Load the JavaScript files, and run the callback for each widget type.
            var scriptsToLoad = [];
            for (var templateKey in data.result.jsIncludes) {
                var templateJs = data.result.jsIncludes[templateKey];

                for (var i = 0; i < templateJs.length; i++) {
                    var jsFile = templateJs[i];

                    if (!self.includedFiles[jsFile]) {
                        self.includedFiles[jsFile] = true;

                        scriptsToLoad.push({
                            file: jsFile,
                            callback: new callbackFn(templateKey)
                        });
                    }
                }//end for
            }//end for

            for (var i = 0; i < scriptsToLoad.length; i++) {
                jQuery.getScript(
                    scriptsToLoad[i].file,
                    scriptsToLoad[i].callback.execute
                );
            }
        });
    }

}

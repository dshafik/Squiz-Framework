function GUIScreenSwitcher(id, settings) {
    GUIContentSwitcher.call(this, id, settings);
    this.className = 'GUIScreenSwitcher';

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
                        var oldScreenid = self.id + '-' + self.currScreen.id;

                        self.currScreen = {
                            system: self.settings.screens[idx].system,
                            id: self.settings.screens[idx].id,
                        },

                        dfx.removeClass(dfx.getId(oldScreenid), 'hover');
                        dfx.addClass(dfx.getId(oldScreenid), 'inactive');
                        dfx.removeClass(dfx.getId(oldScreenid), 'selected');

                        dfx.addClass(this, 'selected');
                        dfx.removeClass(this, 'inactive');
                        dfx.removeClass(this, 'hover');

                        self.loadDynamic(
                            self.settings.screens[idx].system,
                            self.settings.screens[idx].id + 'Screen'
                        );
                    }
                }
            });

            dfx.addEvent(dfx.getId(screenid), 'mouseover', function(evt) {
                if (self.currScreen.id !== self.settings.screens[idx].id) {
                    if (self.settings.screens[idx].disabled !== 'true') {
                        dfx.addClass(this, 'hover');
                    }
                }
            });

            dfx.addEvent(dfx.getId(screenid), 'mouseout', function(evt) {
                if (self.currScreen.id !== self.settings.screens[idx].id) {
                    if (self.settings.screens[idx].disabled !== 'true') {
                        dfx.removeClass(this, 'hover');
                    }
                }
            });

            return true;
        });

    }

};

dfx.noInclusionInherits('GUIScreenSwitcher', 'GUIContentSwitcher', true);

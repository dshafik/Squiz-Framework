function GUIScreenSwitcher(id, settings) {
    this.id       = id;
    this.settings = settings;
    this.elem       = dfx.getId(this.id);
    this.targetid   = this.settings.target;
    this.targetElem = dfx.getId(this.targetid);

    this.currScreenid = null;

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
                self.loadScreen(screenid);
            });

            return true;
        });

    },

    loadScreen: function(id)
    {
        var self   = this;
        var params = {
            systemName: 'Cron',
            screenId:   'CronScreen'
        };

        sfapi.get('GUIScreenSwitcher', 'getScreenContents', params, function(data) {
            dfx.setHtml(self.targetElem, data.result);
            console.info('loadScreen: ', data);
        });
    }

}
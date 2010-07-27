function GUIModeSwitcher(id, settings)
{
    this.id       = id;
    this.settings = settings;


    this.loadedWidgets = [];
    this.targetElement = null;

    if (settings.loadType === 'dynamic') {
        this.targetElement = dfx.getId(settings.target);
    }

    this.init();

}

GUIModeSwitcher.prototype = {

    init: function()
    {
        this.addEvents();

    },

    addEvents: function()
    {
        var parentElem = dfx.getId(this.id);
        var buttons    = dfx.getTag('li', parentElem);

        var self = this;
        dfx.addEvent(buttons, 'click', function(e) {
            self.buttonClicked(e.target);
        });

    },

    buttonClicked: function(button)
    {
        var modeid = button.getAttribute('modeid');
        var system = button.getAttribute('system');
        this.loadMode(system, modeid);

    },

    loadMode: function(system, modeid)
    {
        if (this.settings.loadType === 'static') {
            this.showMode(system, modeid);
        } else {
            this.loadDynamic(system, modeid);
        }

    },

    showMode: function(system, modeid)
    {
        var modeContentElem = dfx.getId(this.id + '-modeContent-' + modeid);
        if (!modeContentElem) {
            // Mode should have been loaded and its element should exist.
            return false;
        }

        dfx.removeClass(dfx.getClass('GUIModeSwitcher-modeContent', dfx.getId(this.id + '-modeContents')), 'visible');
        dfx.addClass(modeContentElem, 'visible');

    },

    loadDynamic: function(system, modeid)
    {
        var self   = this;
        var params = {
            system: system,
            modeid: modeid
        };

        sfapi.get('GUIModeSwitcher', 'getModeContents', params, function(data) {
            // Set the contents.
            dfx.setHtml(self.targetElement, data.result.contents);

            dfx.foreach(data.result.widgets, function(templateKey) {
                var widgets = data.result.widgets[templateKey];
                var wln     = widgets.length;
                for (var i = 0; i < wln; i++) {
                    var widget = widgets[i];
                    self.loadedWidgets.push(widget.id);

                    var widgetObj = eval('new ' + widget.type + '(widget.id, widget.settings)');
                    GUI.addWidget(widget.id, widgetObj);
                }
            });
        });
    }

}
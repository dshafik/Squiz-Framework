function GUIContentSwitcher(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = 'GUIContentSwitcher';


    this.loadedWidgets = [];

    this.init();

}

GUIContentSwitcher.prototype = {

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
        var itemContentElem = dfx.getId(this.id + '-itemContent-' + modeid);
        if (!itemContentElem) {
            // Mode should have been loaded and its element should exist.
            return false;
        }

        var parent = dfx.getId(this.id + '-itemContents');
        if (!parent) {
            // Item contents wrapper not found.
            return false;
        }

        for (var node = parent.firstChild; node; node = node.nextSibling) {
            dfx.removeClass(node, 'visible');
        }

        dfx.addClass(itemContentElem, 'visible');

    },

    loadDynamic: function(system, modeid)
    {
        var self   = this;
        var params = {
            system: system,
            modeid: modeid
        };

        sfapi.get(this.className, 'getDynamicItemContent', params, function(data) {
            // Set the contents.
            dfx.setHtml(dfx.getId(self.settings.target), data.result.contents);

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
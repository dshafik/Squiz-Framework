function GUIContentSwitcher(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = 'GUIContentSwitcher';

    this.loadedWidgets = [];

    this.current = null;

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
        if (!parentElem) {
            return;
        }

        var buttons = dfx.getTag('li', parentElem);

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

        if (this.current !== null) {
            GUI.unloadTemplate(this.current.system + ':' + dfx.ucFirst(this.current.modeid));
        }

        GUI.addTemplate(system + ':' + dfx.ucFirst(modeid));

        this.current = {
            system: system,
            modeid: modeid
        };

    },

    loadDynamic: function(system, modeid)
    {
        var params = {
            system: system,
            modeid: modeid
        };

        GUI.loadContent(this.className, 'getDynamicItemContent', dfx.getId(this.settings.target), params);

        if (this.current !== null) {
            GUI.unloadTemplate(this.current.system + ':' + dfx.ucFirst(this.current.modeid));
        }

        GUI.addTemplate(system + ':' + dfx.ucFirst(modeid));

        this.current = {
            system: system,
            modeid: modeid
        };

    }

}
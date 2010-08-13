function GUIScreenSettings(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.className = 'GUIScreenSettings';

}

GUIScreenSettings.prototype = {

    getValue: function()
    {
        var items = dfx.getClass(this.className + '-item', dfx.getId(this.id));
        var value = {};
        var self  = this;
        dfx.foreach(items, function(i) {
            var item         = items[i];
            var widgetValues = self.getWidgetValues(item);
            dfx.foreach(widgetValues, function(widgetid) {
                value[widgetid] = widgetValues[widgetid];
            });
        });

        return value;

    },

    getWidgetValues: function(itemElem)
    {
        // Find all the elements with ids.
        var elems   = dfx.find(itemElem, '[id]');
        var ln      = elems.length;
        var values  = {};

        if (ln === 0) {
            return null;
        }

        for (var i = 0; i < ln; i++) {
            var elem = elems[i];
            var id   = elem.getAttribute('id');
            if (!id) {
                continue;
            }

            // Check if id is a valid widget id.
            var widget = GUI.getWidget(id);
            if (!widget) {
                continue;
            }

            // TODO: Should we only return top level widgets?
            values[id] = widget.getValue();
        }//end for

        return values;

    }

};

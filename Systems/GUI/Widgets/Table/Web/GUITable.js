function GUITable(id, settings) {
    this.id       = id;
    this.settings = settings;

    this.init();
}

GUITable.prototype = {

    init: function()
    {
        if (this.settings.sortable === 'true') {
            $("#" + this.id + "").tablesorter();
        }

        var self      = this;
        var tableElem = dfx.getId(this.id);

        if (this.settings.selectable === 'true') {
            // Rows are selectable add the click event.
            dfx.addEvent(tableElem, 'click', function(e) {
                var target = dfx.getMouseEventTarget(e);
                while (target && dfx.isTag(target, 'table') === false) {
                    if (dfx.isTag(target, 'td') === true) {
                        self._columnClicked(target);
                        // Note that we do not break here so that rowClicked event is
                        // also fired.
                    } else if (dfx.isTag(target, 'tr') === true) {
                        self._rowClicked(target);
                        break;
                    }

                    target = target.parentNode;
                }
            });
        }
    },

    _columnClicked: function(col)
    {

    },

    _rowClicked: function(row)
    {
        if (!row) {
            return;
        }

        var parent = row.parentNode;
        for (var node = parent.firstChild; node; node = node.nextSibling) {
            dfx.removeClass(node, 'selected');
        }

        dfx.addClass(row, 'selected');

    }

};

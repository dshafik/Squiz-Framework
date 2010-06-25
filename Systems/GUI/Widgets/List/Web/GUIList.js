function GUIList(id, settings) {

    this.id       = id;
    this.settings = settings;
    this.init();

}

GUIList.prototype = {
    init: function()
    {
        // Setup sorting.
        if (this.settings.sortable === 'true') {
            this.enableSorting();
        }
    },

    enableSorting: function()
    {
        jQuery('#' + this.id).sortable({
            handle: 'span.GUIList-dragHandle',
            axis: 'y'
        });

    }

};

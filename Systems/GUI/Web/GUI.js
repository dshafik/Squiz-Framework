var GUI = new function()
{
    this.widgetStore   = {};
    var _publicDirName = 'Web';

    this.getWidget = function(id) {
        return this.widgetStore[id];

    };

    this.addWidget = function(id, obj) {
        this.widgetStore[id] = obj;

    };

    this.getWidgetWebPath = function(widgetPath) {
        widgetPath = widgetPath.replace(/^\//, '');
        widgetPath = widgetPath.replace('/', '/Widgets/') + '/' + _publicDirName;
        return widgetPath;

    };

    this.getWidgetURL = function(widgetPath) {
        var url = window.location.href.replace(/\/+$/, '');
        url    += '/__web/Systems/' + GUI.getWidgetWebPath(widgetPath);
        return url;

    };

};

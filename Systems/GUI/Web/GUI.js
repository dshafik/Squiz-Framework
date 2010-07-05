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

    /**
     * Add event handler functions to the passed object.
     *
     * The three event handling function will be added to the object
     * passed in.
     *
     * @param {Object} obj Widget object to add events..
     * @param {String} eventName Custom event name to use.
     * @return {Void}
     */
    this.addWidgetEvent = function(obj, eventName) {
        var self = this;

        if (!obj.__eventCallbacks) {
            obj.__eventCallbacks = {};
        }

        if (!obj.__eventCallbacks[eventName]) {
            obj.__eventCallbacks[eventName] = [];
        } else {
            return;
        }

        var addEvtid = 'add' + dfx.ucFirst(eventName) + 'Callback';
        obj[addEvtid] = function(callback) {
            if (typeof callback === 'function') {
                obj.__eventCallbacks[eventName].push(callback);
            }
        };

        var fireEvtid = 'fire' + dfx.ucFirst(eventName) + 'Callbacks';
        obj[fireEvtid] = function() {
            if (obj.__eventCallbacks) {
                var len = obj.__eventCallbacks[eventName].length;
                for (var i = 0; i < len; i++) {
                    obj.__eventCallbacks[eventName][i].call(obj);
                }
            }
        };

        var rmEvtid = 'removeAll' + dfx.ucFirst(eventName) + 'Callbacks';
        obj[rmEvtid] = function() {
            obj.__eventCallbacks[eventName] = [];
        };

    };

};

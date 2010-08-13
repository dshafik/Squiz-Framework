/**
 * JS Class for GUI system.
 *
 * This includes generic function for handling widgets, templates and so on.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var GUI = new function()
{
    this.widgetStore     = {};
    var _publicDirName   = 'Web';
    var _modifiedWidgets = {};
    var _reverting       = false;
    this.widgetTemplates = {};
    this.templateLineage = [];

    this.getWidget = function(id) {
        return this.widgetStore[id];
    };

    this.addWidget = function(id, obj) {
        if (!obj.id || !obj.settings || !obj.settings.template) {
            // Invalid widget.
            return;
        }

        this.widgetStore[id] = obj;

        var parent = obj.settings.template.system + ':' + obj.settings.template.name;

        if (!this.widgetTemplates[parent]) {
            this.widgetTemplates[parent] = {};
        }

        this.widgetTemplates[parent][id] = obj;
    };

    this.removeWidget = function(id) {
        if (this.widgetStore[id]) {
            if (this.widgetStore[id].removeWidget) {
                this.widgetStore[id].removeWidget();
            }

            this.widgetStore[id] = null;
            delete this.widgetStore[id];
            return true;
        }

        return false;
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

    this.unloadTemplate = function(parent) {
        var self = this;
        var ln   = this.templateLineage.length;
        for (var i = (ln - 1); i >= 0; i--) {
            var p = this.templateLineage[i];
            dfx.foreach(this.widgetTemplates[p], function(idx) {
                self.removeWidget(idx);
            });

            delete self.widgetTemplates[p];
            this.templateLineage.pop();

            if (p === parent) {
                break;
            }
        }
    };

    this.addTemplate = function(template) {
        this.templateLineage.push(template);
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

        var addEvtid  = 'add' + dfx.ucFirst(eventName) + 'Callback';
        obj[addEvtid] = function(callback) {
            if (typeof callback === 'function') {
                obj.__eventCallbacks[eventName].push(callback);
            }
        };

        var fireEvtid  = 'fire' + dfx.ucFirst(eventName) + 'Callbacks';
        obj[fireEvtid] = function() {
            if (obj.__eventCallbacks) {
                var len = obj.__eventCallbacks[eventName].length;
                for (var i = 0; i < len; i++) {
                    obj.__eventCallbacks[eventName][i].apply(obj, arguments);
                }
            }
        };

        var rmEvtid  = 'removeAll' + dfx.ucFirst(eventName) + 'Callbacks';
        obj[rmEvtid] = function() {
            obj.__eventCallbacks[eventName] = [];
        };
    };

    this.sendRequest = function(system, action, params, callback) {
        sfapi.get(system, action, params, callback);
    };

    /**
     * Loads content in to the specified element or elementid.
     *
     * Result from the called method must have result.contents and result.widgets.
     * All widgets that are contained in the content will be initialised.
     */
    this.loadContent = function(system, method, targetElement, params) {
        if (dfx.isObj(targetElement) === false) {
            targetElement = dfx.getId(targetElement);
        }

        GUI.sendRequest(system, method, params, function(data) {
            // Set the contents.
            dfx.setHtml(targetElement, data.result.contents);
        });
    };

    this.save = function() {
        var self   = this;
        var values = {};
        var ln     = this.templateLineage.length;

        // Retrieve the save values in reverse order.
        for (var i = (ln - 1); i >= 0; i--) {
            var template     = this.templateLineage[i];
            var isEmpty      = true;
            values[template] = {};
            dfx.foreach(self.widgetTemplates[template], function(widgetid) {
                var widget = self.getWidget(widgetid);
                if (!widget || !widget.getValue) {
                    // Invalid object ref. or widget has no getValue function.
                    return;
                }

                isEmpty = false;
                values[template][widgetid] = widget.getValue();
            });

            if (isEmpty === true) {
                // Nothing to save for this template.
                delete values[template];
            }
        }//end for

        console.info(values);
        var params = {
            templateData: dfx.jsonEncode(values)
        };

        GUI.sendRequest('GUI', 'saveTemplateData', params, function() {
            console.info(arguments);
        });

        return values;
    };

    this.revert = function() {
        _reverting = true;
        dfx.foreach(_modifiedWidgets, function(widgetid) {
            var widget = GUI.getWidget(widgetid);
            if (widget && widget.revert) {
                widget.revert();
            }
        });

        _modifiedWidgets = {};
        _reverting = false;

    };

    this.setModified = function(widget, state) {
        if (_reverting === true) {
            return;
        }

        if (state === true) {
            _modifiedWidgets[widget.id] = true;
        } else if (_modifiedWidgets[widget.id] === true) {
            delete _modifiedWidgets[widget.id];
        }

        this.fireModifiedCallbacks(widget.id, state);
    };


    this.addWidgetEvent(this, 'modified');

};

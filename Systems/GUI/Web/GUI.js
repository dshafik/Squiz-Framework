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
    this.widgetStore       = {};
    var _publicDirName     = 'Web';
    var _modifiedWidgets   = {};
    var _modifiedTemplates = {};
    var _queuedOverlays    = {
        error: [],
        warning: [],
        saved: [],
        loading: [],
        normal: []
    };
    var _reverting         = false;
    this.widgetTemplates   = {};
    this.templateLineage   = [];

    this.getWidget = function(id) {
        return this.widgetStore[id];
    };

    window.onbeforeunload = function(e) {
        if (GUI.isTemplateModified() === true) {
            return 'You have unsaved changes!!!';
        }
    };

    this.addWidget = function(id, obj) {
        if (!obj.id || !obj.settings || !obj.settings.template) {
            return;
        }

        this.widgetStore[id] = obj;

        var parent = obj.settings.template.system + ':' + obj.settings.template.name;

        if (!this.widgetTemplates[parent]) {
            this.widgetTemplates[parent] = {};
        }

        this.widgetTemplates[parent][id] = obj;

        // Also if the lineage is empty then we need to add the parent template as the
        // initial lineage item.
        if (this.templateLineage.length === 0) {
            this.addTemplate(parent);
        }
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

        if (_modifiedWidgets[id]) {
            delete _modifiedWidgets[id];
        }

        return false;
    };

    this.createWidget = function(type, id, settings, creator) {
        var tplInfo = null;

        if (typeof creator === 'string') {
            // Creator could be a template.
            var parts = creator.split(':');
            if (parts.length === 2) {
                tplInfo = {
                    system: parts[0],
                    name: parts[1]
                };
            }
        } if (creator && creator.settings && creator.settings.template) {
            tplInfo = creator.settings.template;
        }

        if (!window[type]) {
            GUI.message('developer', 'Widget type "' + type + '" class not loaded!');
            return;
        } else if (!tplInfo || !tplInfo.system || !tplInfo.name) {
            GUI.message('developer', 'Invalid argument: creator must be a valid widget object or a template identifier.');
            return;
        }

        if (!settings) {
            settings = {};
        }

        settings.widget = {
            id: id,
            type: type
        };

        settings.template = tplInfo;

        // Create the widget object and then add it to widget store.
        var widgetObject = null;
        eval('widgetObject = new ' + type + '(id, settings);');

        if (!widgetObject) {
            GUI.message('developer', 'Failed to create widget object of type ' + type + ' and id ' + id);
            return;
        }

        this.addWidget(id, widgetObject);
        return widgetObject;
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

    this.unloadTemplate = function(parent, force) {
        var self = this;
        var ln   = this.templateLineage.length;
        if (this.templateLineage.inArray(parent) === true) {
            for (var i = (ln - 1); i >= 0; i--) {
                var p = this.templateLineage[i];
                if (force !== true && this.isTemplateModified(p) === true) {
                    if (this.confirmUnload() === false) {
                        return false;
                    }
                }

                dfx.foreach(this.widgetTemplates[p], function(idx) {
                    self.removeWidget(idx);
                });

                delete self.widgetTemplates[p];
                this.templateLineage.pop();
                if (_modifiedTemplates[p]) {
                    delete _modifiedTemplates[p];
                }

                if (p === parent) {
                    this.fireTemplateRemovedCallbacks(parent);
                    break;
                }
            }
        }
    };

    this.isTemplateModified = function(template, checkChildTemplates) {
        if (_reverting === true) {
            // If we are reverting template data and template is being reloaded
            // this method may get called so return false.
            return false;
        }

        template = template || GUI.getCurrentTemplate();

        if (checkChildTemplates === true) {
            if (this.templateLineage.inArray(template) === true) {
                var ln = this.templateLineage.length;
                for (var i = (ln - 1); i >= 0; i--) {
                    var p = this.templateLineage[i];
                    if (this.isTemplateModified(p, false) === true) {
                        return true;
                    }

                    if (p === template) {
                        return false;
                    }
                }
            }
        } else if (_modifiedTemplates[template]) {
            return true;
        }

        return false;
    };

    this.hasModifiedTemplates = function() {
        return !dfx.isEmpty(_modifiedTemplates);
    };

    this.hasModifiedWidgets = function() {
        return !dfx.isEmpty(_modifiedWidgets);
    };

    this.confirmUnload = function() {
        var msg = 'You have unsaved changes.\n\nPress OK to continue and lose these changes or Cancel to stay on the current screen.';
        return confirm(msg);
    };

    this.addTemplate = function(template) {
        if (dfx.inArray(template, this.templateLineage) === false) {
            this.templateLineage.push(template);
            this.fireTemplateAddedCallbacks(template);
        }
    };

    this.getCurrentTemplate = function() {
        return this.templateLineage[(this.templateLineage.length - 1)];
    };

    this.getTemplateLineage = function() {
        return this.templateLineage;
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

    this.sendRequest = function(system, action, params, callback, format, errorCallback) {
        var self = this;
        sfapi.post(system, action, params, function(data) {
            if (dfx.isset(data) === false
                || data.exception
                || data.toString().indexOf('exception:') === 0
            ) {
                var exception = '';
                if (data.exception) {
                    exception = data.exception;
                } else if (data.indexOf('exception:') === 0) {
                    exception = data.replace('exception:', '');
                }

                if (!exception) {
                    exception = 'Invalid response';
                }

                // Show error message.
                GUI.queueOverlay({
                    icon: 'error',
                    content: '<p>' + exception + '</p><br /><button class="GUIButton buttonDark" type="button" onclick="javascript:GUI.dequeueOverlay(\'error\');">Close</button>'
                });

                if (errorCallback) {
                    errorCallback.call(this, exception, data);
                }
            } else if (callback) {
                callback.call(this, data);
            }

            self.fireRequestCompleteCallbacks();
        }, errorCallback, format);
    };

    this.getRequestURL = function(system, action, params, format) {
        var url = sfapi.createURL(system, action, format);
        url     = dfx.addToQueryString(url, params);
        url     = sfapi.attachTokenToURL(url);
        return url;
    };

    /**
     * Loads content in to the specified element or elementid.
     *
     * Result from the called method must have result.contents and result.widgets.
     * All widgets that are contained in the content will be initialised.
     *
     * @param {string}     system        Name of the system that has the action.
     * @param {string}     method        Name of the API method.
     * @param {DOMElement} targetElement The DOMElement that will wrap the content.
     * @param {object}     params        Parameters for the API method.
     */
    this.loadContent = function(system, method, targetElement, params, callback) {
        if (dfx.isObj(targetElement) === false) {
            targetElement = dfx.getId(targetElement);
        }

        GUI.sendRequest(system, method, params, function(contents) {
            // Set the contents.
            dfx.setHtml(targetElement, contents);
            if (callback) {
                callback.call(this);
            }
        }, 'raw');
    };

    /**
     * Loads specified template in to an element.
     *
     * Once the contents are loaded the element that was created will be passed to
     * the callback function where it can be modified before displayed on the screen.
     *
     * Available options:
     * - [modal] (true|false): If true then the template contents will be shown as
     * modal dialog. False by default.
     * - [targetElement] (HTMLElement): If specified, the contents of the template
     * will be appended to this element's contents. If omitted, the document body
     * will be used.
     *
     * @param string   system       The system that has the template.
     * @param string   templateName Name of the template (with the extension).
     * @param function callback     The function to call after the contents are loaded.
     * @param object   options      List of options.
     *
     * @return void
     */
    this.loadTemplate = function(system, templateName, templateOptions, callback, options) {
        var params = {
            systemName: system,
            template: templateName,
            templateOptions: dfx.jsonEncode(templateOptions)
        };

        if (options && options.modal === true) {
            GUI.queueOverlay();
        }

        if ((options) && (options.targetElement)) {
            var targetElement = options.targetElement;
        } else {
            var targetElement = document.body;
        }

        GUI.sendRequest('GUI', 'printTemplate', params, function(content) {
            if (!content) {
                GUI.message('developer', 'Failed to load template content', 'error');
                return;
            }

            // Create the element where the content will be set.
            var main = document.createElement('div');

            // Hide the element and add it to body so that events and DOM operations
            // can work.
            targetElement.appendChild(main);
            dfx.setHtml(main, content);

            if (options && options.modal === true) {
                GUI.addTemplate(system + ':' + templateName.replace(/.tpl$/, ''));
            }

            if (callback) {
                callback.call(this, main);
            }
        }, 'raw');
    };

    this.reloadTemplate = function(template) {
        this.fireReloadTemplateCallbacks(template);
    };

    this.save = function() {
        // Show loader.
        GUI.queueOverlay({
            icon: 'loading',
            content: 'Saving...'
        });

        var self   = this;
        var values = {};
        var ln     = this.templateLineage.length;

        // Retrieve the save values in reverse order.
        for (var i = (ln - 1); i >= 0; i--) {
            var template     = this.templateLineage[i];
            var isEmpty      = true;
            values[template] = {};

            // Template it self may have a getValue function.
            // The templateData variable is a reserved var.
            var tplSystemName   = template.split(':')[0];
            var tplTemplateName = template.split(':')[1];
            var tplClassName    = tplSystemName + tplTemplateName;

            if (window[tplClassName] && window[tplClassName].getValue) {
                values[template].templateData = window[tplClassName].getValue();
                isEmpty = false;
            }

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

        var params = {
            templateData: dfx.jsonEncode(values)
        };

        GUI.sendRequest('GUI', 'saveTemplateData', params, function(data) {
            if (!data.result) {
                // TODO: Failed to save. Invalid result, most likely a PHP error.
            } else if (data.result.errors) {
                // Send the errors to template.
                self.queueOverlay({
                    icon: 'warning',
                    content: data.result.errorList
                });
                self.dequeueOverlay('loading');
            } else if (data.result.success) {
                dfx.foreach(data.result.success, function(templateKey) {
                    var retVal = data.result.success[templateKey];
                    // For each template that returned a success call their "saved"
                    // method to perform extra actions.
                    var tplSystemName   = templateKey.split(':')[0];
                    var tplTemplateName = templateKey.split(':')[1];
                    var tplClassName    = tplSystemName + tplTemplateName;

                    var saveChildren = true;
                    if (window[tplClassName] && window[tplClassName].saved) {
                        saveChildren = window[tplClassName].saved(retVal);
                    }

                    if (saveChildren !== false) {
                        // The saved method of a template class can return false
                        // to prevent saved method of child widget being called.
                        dfx.foreach(self.widgetTemplates[templateKey], function(widgetid) {
                            var widget = self.widgetTemplates[templateKey][widgetid];
                            if (widget && widget.saved) {
                                widget.saved();
                            }
                        });
                    }

                    // Show saved icon for a second.
                    self.queueOverlay({
                        icon: 'saved',
                        content: 'Save Complete'
                    });
                    self.dequeueOverlay('loading');
                    setTimeout(function() {
                        self.dequeueOverlay('saved');
                        _modifiedWidgets   = {};
                        _modifiedTemplates = {};
                        self.fireSavedCallbacks();
                    }, 1000);
                });
            } else {
                // TODO: Unknown return type.
            }//end if
        });

        return values;
    };

    this.revert = function() {
        _reverting = true;

        var ln = this.templateLineage.length;

        for (var i = (ln - 1); i >= 0; i--) {
            var template = this.templateLineage[i];

            // Template it self may have a getValue function.
            // The templateData variable is a reserved var.
            var tplSystemName   = template.split(':')[0];
            var tplTemplateName = template.split(':')[1];
            var tplClassName    = tplSystemName + tplTemplateName;

            var revertTemplate = true;
            if (window[tplClassName] && window[tplClassName].revert) {
                revertTemplate = window[tplClassName].revert();
            }

            if (revertTemplate === false) {
                dfx.foreach(_modifiedWidgets, function(widgetid) {
                    var widget = GUI.getWidget(widgetid);
                    if (widget
                        && widget.revert
                        && widget.settings.template.system === tplSystemName
                        && widget.settings.template.name === tplTemplateName
                    ) {
                        widget.revert();
                    }
                });
            } else if (i === (ln - 1)) {
                // Reload the top template.
                GUI.reloadTemplate(template);
            }
        }

        _modifiedWidgets   = {};
        _modifiedTemplates = {};
        _reverting         = false;

        this.fireRevertedCallbacks();
    };

    this.setModified = function(widget, state, dontFireEvents) {
        if (_reverting === true) {
            return;
        }

        if (typeof widget === 'string') {
            // If we are passed a string in place of the widget, we believe we
            // are being sent a template.
            if (state === true) {
                _modifiedTemplates[widget] = true;
            } else if (_modifiedTemplates[widget] === true) {
                delete _modifiedTemplates[widget];

                // Also set the child widgets to not modified.
                var self = this;
                dfx.foreach(this.widgetTemplates[widget], function(idx) {
                    self.setModified(self.widgetTemplates[widget][idx], false, true);
                });
            }

            if (dontFireEvents !== true) {
                this.fireModifiedCallbacks(widget, state);
            }
        } else {
            // It's an object, hence we believe this is a widget.
            if (state === true) {
                _modifiedWidgets[widget.id] = true;

                // Set the template of this widget as modified as well.
                // And add it to the lineage since it requires a save.
                if (widget.settings && widget.settings.template) {
                    var newTemplate = widget.settings.template.system + ':' + widget.settings.template.name;
                    _modifiedTemplates[newTemplate] = true;
                    this.addTemplate(newTemplate);
                }
            } else if (_modifiedWidgets[widget.id] === true) {
                delete _modifiedWidgets[widget.id];
            }

            if (dontFireEvents !== true) {
                this.fireModifiedCallbacks(widget.id, state);
            }
        }
    };

    /**
     * Displays the specified message to user or developer.
     *
     * @param string type    The type of the message. Valid values are "user", "developer".
     * @param string message The message to display.
     * @param string level   The message level. Valid valus are "warning", "error".
     *
     * @return void
     */
    this.message = function(type, message, level) {
        switch (type) {
            case 'developer':
                if (window.console) {
                    if (level === 'warning' && window.console.warn) {
                        console.warn(message);
                    } else if (window.console.error) {
                        console.error(message);
                    }
                }
            break;

            case 'user':
                // TODO: Show the message box to user.
                alert(message);
            break;

            default:
                // Ignore.
            break;
        }
    };

    /**
     * Displays an overlay on top of every element on the screen or elements inside specified element.
     *
     * @param {object}  [options] List of options for the overlay. Options:
     *                            - icon: loading|saved|warning|error.
     *                            - content: HTML string to show in the overlay.
     *
     * @param {DOMNode} [target=document.body] The element that will have the overlay.
     *
     * @returns {void}
     */
    var _showOverlay = function(options, target) {
        if (!target) {
            target = document.body;
        }

        options = options || {};

        var overlayid = 'GUI-overlay';
        var overlay   = dfx.getId(overlayid);
        if (!overlay) {
            overlay    = document.createElement('div');
            overlay.id = overlayid;
            dfx.addClass(overlay, 'GUI-overlay');
        }

        var content = '<div class="GUI-overlayOpacity"></div>';

        if (options.icon) {
            dfx.addClass(overlay, 'GUI-overlay-icon');

            switch (options.icon) {
                case 'loading':
                    content += '<img src="/__web/Systems/GUI/Web/loading_black.gif" class="GUI-overlay-icon" id="GUI-overlay-loading" />';
                break;

                default:
                    content += '<img src="/__web/Systems/GUI/Web/' + options.icon + '.png" class="GUI-overlay-icon" id="GUI-overlay-' + options.icon + '" />';
                break;
            }
        }//end if

        if (options.content) {
            content += '<div id="GUI-overlay-content">';
            content += options.content;
            content += '</div>';
        }

        dfx.setHtml(overlay, content);

        target.appendChild(overlay);
    };

    var _hideOverlay = function() {
        var elem = dfx.getId('GUI-overlay');
        if (elem) {
            dfx.remove(elem);
        }
    };

    var _showCurrentOverlay = function() {
        var currentOverlay = null;

        for (i in _queuedOverlays) {
            var thisQueue = _queuedOverlays[i];

            if (thisQueue.length > 0) {
                // Get the front of the queue.
                currentOverlay = thisQueue[0];
                break;
            }
        }//end for

        if (currentOverlay === null) {
            _hideOverlay();
        } else {
            _showOverlay(currentOverlay.options, currentOverlay.target);
        }
    };

    this.queueOverlay = function(options, target) {
        options = options || {};

        var type = options.icon;
        if (!type) {
            type = 'normal';
        }

        if (!target) {
            target = document.body;
        }

        var overlay = {
            options: options,
            target: target
        };

        _queuedOverlays[type].push(overlay);
        _showCurrentOverlay();
    };

    this.dequeueOverlay = function(type) {
        if (type === null) {
            type = 'normal';
        }

        if (_queuedOverlays[type].length > 0) {
            _queuedOverlays[type].shift();
        }//end for

        _showCurrentOverlay();
    };

    this.addWidgetEvent(this, 'modified');
    this.addWidgetEvent(this, 'reverted');
    this.addWidgetEvent(this, 'saved');
    this.addWidgetEvent(this, 'requestComplete');
    this.addWidgetEvent(this, 'reloadTemplate');
    this.addWidgetEvent(this, 'templateAdded');
    this.addWidgetEvent(this, 'templateRemoved');

};

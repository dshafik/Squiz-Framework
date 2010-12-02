/**
 * JS Class for the Viper Plugin Manager.
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
 * @package    CMS
 * @subpackage Editing
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var ViperPluginManager = {

    plugins: {},
    pluginConstructors: {},
    keyPressListeners: {},
    viper: null,
    activePlugin: null,
    pluginSets: {},
    activePluginSet: null,
    pluginsURL: '',
    callbacks: {},
    allowTextInput: false,
    _pluginSettings: {},

    init: function(viper)
    {
        this.viper = viper;
        this.loadAllPluginStyles();

    },

    getPluginNames: function()
    {
        var plugins = [];
        for (var name in this.plugins) {
            plugins.push(name);
        }

        return plugins;

    },

    loadAllPluginStyles: function()
    {
        var self = this;
        dfx.foreach(this.pluginConstructors, function(pluginName) {
            self.loadPluginStyles(pluginName);
        });

    },

    loadPluginStyles: function(plugin)
    {
        if (dfx.isset(plugin) === true) {
            var url = this.getPluginStylesURL(plugin) + '/' + plugin + '.css';
            dfx._includeCss(url);
        }

    },

    getPluginStylesURL: function(plugin)
    {
        var url = this.pluginsURL + plugin;
        return url;

    },

    addPluginObject: function(pluginName, pluginObj)
    {
        this.plugins[pluginName] = pluginObj;

    },

    addPlugin: function(name, pluginConstructor)
    {
        if (typeof pluginConstructor !== 'function') {
            throw Error('ViperPluginException: plugin must be a constructor function');
        }

        if (dfx.isset(this.pluginConstructors[name]) === false) {
            this.pluginConstructors[name] = pluginConstructor;
            this.loadPluginStyles(name);
        }

    },

    loadPluginScripts: function(plugins, callback)
    {
        if (plugins.length === 0) {
            callback.call(this);
        } else {
            var plugin = plugins.shift();
            if (typeof plugin === 'object') {
                plugin = plugin.name;
            }

            if (dfx.isset(window[plugin]) === true) {
                if (dfx.isset(window[plugin]) === true) {
                    ViperPluginManager.addPlugin(plugin, window[plugin]);
                }

                this.loadPluginScripts(plugins, callback);
            } else {
                var self = this;
                var url  = this.pluginsURL + plugin + '/' + plugin + '.js';
                dfx.includeScript(url, function() {
                    if (dfx.isset(window[plugin]) === true) {
                        ViperPluginManager.addPlugin(plugin, window[plugin]);
                    }

                    self.loadPluginScripts(plugins, callback);
                });
            }
        }//end if

    },

    setActivePlugin: function(name, allowTextInput)
    {
        allowTextInput      = allowTextInput || false;
        this.activePlugin   = name;
        this.allowTextInput = allowTextInput;

    },

    getActivePlugin: function()
    {
        return this.activePlugin;

    },

    _getPluginName: function(pluginConstructor)
    {
        var fn    = pluginConstructor.toString();
        var start = 'function '.length;
        var name  = fn.substr(start, (fn.indexOf('(') - start));
        return name;

    },

    /**
     * Removes specified plugin.
     *
     * @param {string} plugin Name of the plugin.
     *
     * @return void
     */
    removePlugin: function(plugin)
    {
        if (this.plugins[plugin]) {
            // Call the remove fn of the plugin incase it needs to do cleanup.
            this.plugins[plugin].remove();

            // Remove the keyPress listeners for this plugin.
            this.removeKeyPressListener(this.plugins[plugin]);
            this.fireCallbacks('pluginRemoved', plugin);
        }

    },

    /**
     * Returns the plugin object for specified plugin name.
     *
     * @param {string} name Name of the plugin.
     *
     * @return The Viper plugin object.
     * @type {ViperPlugin}
     */
    getPlugin: function(name)
    {
        return this.plugins[name];

    },

    /**
     * Add a new set of plugins.
     *
     * @param {string}   name     Name of the plugin set (e.g. "simpleEditing").
     * @param {array}    plugins  List of plugin names that are in the set.
     * @param {boolean}  load     If true then script files will be loaded.
     * @param {function} callback Call back function.
     *
     * @return void
     */
    addPluginSet: function(name, plugins, load, callback)
    {
        if (dfx.isset(plugins) === true) {
            this.pluginSets[name] = plugins;
        } else {
            this.pluginSets[name] = [];
        }

        if (load === true) {
            var clone = this.pluginSets[name].concat([]);
            this.loadPluginScripts(clone, function() {
                if (callback) {
                    callback.call(this);
                }
            });
        } else if (callback) {
            callback.call(this);
        }

    },

    /**
     * Remove a set of plugins.
     */
    removePluginSet: function(name)
    {
        if (this.pluginSetExists(name) === true) {
            delete this.pluginSets[name];
        }

    },

    /**
     * Adds specified plugin to the given plugin set.
     */
    addPluginToSet: function(setName, pluginName)
    {
        if (this.pluginSetExists(name) === true) {
            this.pluginSets[setName].push(pluginName);
        }

    },

    /**
     * Returns true if specified set name is valid set.
     */
    pluginSetExists: function(name)
    {
        return dfx.isset(this.pluginSets[name]);

    },

    /**
     * Enables only the plugins that are in the specified set.
     * All other plugins are removed or disabled.
     *
     * @param string  name        Name of the plugin set.
     * @param boolean disableOnly Plugins not in the specifed set will be disabled.
     */
    usePluginSet: function(setName, disableOnly, callback)
    {
        if (this.pluginSetExists(setName) === true) {
            // Load the scripts.
            var self  = this;
            var clone = this.pluginSets[setName].concat([]);
            this.loadPluginScripts(clone, function() {
                // Plugin files are loaded.
                // If there is an active plugin set then remove/disable them.
                if (self.activePluginSet !== null) {
                    self._removeSet(self.activePluginSet);
                }

                // Enable the plugins in the new set.
                self._enableSet(setName);

                if (dfx.isFn(callback) === true) {
                    callback.call(this, setName);
                }
            });
        } else if (this.activePluginSet !== null) {
            this._removeSet(this.activePluginSet);
        }//end if

    },

    _removeSet: function(name)
    {
        var pSetLen = this.pluginSets[name].length;
        for (var i = 0; i < pSetLen; i++) {
            var plugin = this.pluginSets[name][i];
            if (typeof plugin === 'object') {
                plugin = plugin.name;
            }

            this.removePlugin(plugin);
        }

        this.activePluginSet = null;

    },

    _enableSet: function(name)
    {
        this.activePluginSet = name;
        var pSetLen = this.pluginSets[name].length;
        for (var i = 0; i < pSetLen; i++) {
            var plugin     = this.pluginSets[name][i];
            var pluginName = '';
            if (typeof plugin === 'object') {
                pluginName = plugin.name;
            } else {
                pluginName = plugin;
            }

            var pluginConstructor = this.pluginConstructors[pluginName];
            if (pluginConstructor) {
                var pluginObj            = new pluginConstructor(this.viper);
                this.plugins[pluginName] = pluginObj;

                if (dfx.isset(plugin.settings) === true) {
                    pluginObj.setSettings(plugin.settings);
                } else if (this._pluginSettings[pluginName]) {
                    pluginObj.setSettings(this._pluginSettings[pluginName]);
                }

                pluginObj.start();
            }
        }//end for

    },

    setPluginSettings: function(pluginName, settings)
    {
        if (this.plugins[pluginName]) {
            this.plugins[pluginName].setSettings(settings);
        }

        this._pluginSettings[pluginName] = settings;

    },

    disablePlugin: function(name)
    {
        this.plugins[name].disable();

    },

    isPluginElement: function(element)
    {
        for (var i in this.plugins) {
            if (this.plugins[i].isPluginElement) {
                if (this.plugins[i].isPluginElement(element) === true) {
                    return true;
                }
            }
        }

        return false;

    },

    addKeyPressListener: function(keys, plugin, evtFunction, data)
    {
        var keysArray = keys.toLowerCase().split('+').sort();
        var listener  = {
            plugin: plugin,
            fn: evtFunction,
            data: data
        };

        var keysStr = keysArray.join('+');
        if (!this.keyPressListeners[keysStr]) {
            this.keyPressListeners[keysStr] = [];
        }

        this.keyPressListeners[keysStr].push(listener);

    },

    removeKeyPressListener: function(plugin, keys)
    {
        if (plugin && keys && this.keyPressListeners[keys]) {
            var ln = this.keyPressListeners[keys].length;
            for (var i = 0; i < ln; i++) {
                var listener = this.keyPressListeners[keys][i];
                if (listener.plugin === plugin) {
                    this.keyPressListeners[keys].splice(i, 1);
                    break;
                }
            }
        } else if (plugin) {
            // Remove all key listeners for a specific plugin.
            for (var k in this.keyPressListeners) {
                if (this.keyPressListeners.hasOwnProperty(k) === false) {
                    continue;
                }

                var ln = this.keyPressListeners[k].length;
                if (ln === 0) {
                    delete this.keyPressListeners[k];
                } else {
                    for (var i = 0; i < ln; i++) {
                        var listener = this.keyPressListeners[k][i];
                        if (listener.plugin === plugin) {
                            this.keyPressListeners[k].splice(i, 1);
                            break;
                        }
                    }
                }
            }
        }//end if

    },

    fireKeyPressed: function(e)
    {
        if (this._fireKeyPressFns(e, 'all_keys') === false) {
            return false;
        }

        var eKeys = [];
        if (e.ctrlKey === true || e.metaKey === true) {
            eKeys.push('ctrl');
        }

        if (e.shiftKey === true) {
            eKeys.push('shift');
        }

        if (e.altKey === true) {
            eKeys.push('alt');
        }

        switch (e.keyCode) {
            case 13:
                eKeys.push('enter');
            break;

            case dfx.DOM_VK_LEFT:
                eKeys.push('left');
            break;

            case dfx.DOM_VK_RIGHT:
                eKeys.push('right');
            break;

            case dfx.DOM_VK_UP:
                eKeys.push('up');
            break;

            case dfx.DOM_VK_DOWN:
                eKeys.push('down');
            break;

            case 9:
                eKeys.push('tab');
            break;

            case dfx.DOM_VK_DELETE:
                eKeys.push('delete');
            break;

            default:
                var code;
                if (e.keyCode) {
                    code = e.keyCode;
                } else if (e.which) {
                    code = e.which;
                }

                // Other characters (a-z0-9..).
                if (code) {
                    eKeys.push(String.fromCharCode(code).toLowerCase());
                }
            break;
        }//end switch

        var eKeysStr = eKeys.sort().join('+');

        return this._fireKeyPressFns(e, eKeysStr);

    },

    _fireKeyPressFns: function(e, eKeysStr)
    {
        if (this.keyPressListeners[eKeysStr]) {
            var ln = this.keyPressListeners[eKeysStr].length;
            for (var i = 0; i < ln; i++) {
                var listener = this.keyPressListeners[eKeysStr][i];
                var eventFn  = listener.fn;
                var plugin   = listener.plugin;
                var data     = listener.data;

                if (eventFn) {
                    if (dfx.isFn(eventFn) === true) {
                        if (eventFn.call(plugin, e, data) === true) {
                            dfx.preventDefault(e);
                            return false;
                        }
                    } else if (plugin[eventFn] && plugin[eventFn].call(plugin, e, data) === true) {
                        dfx.preventDefault(e);
                        return false;
                    }
                }
            }
        }//end if

        return true;

    },

    fireSelectionChanged: function(range)
    {
        for (var i in this.plugins) {
            this.plugins[i].selectionChanged(range);
        }

    },

    fireNodeInserted: function(node, range)
    {
        for (var i in this.plugins) {
            if (this.plugins[i].nodeInserted(node, range) === false) {
                return false;
            }
        }

    },

    fireCaretPositioned: function()
    {
        for (var i in this.plugins) {
            this.plugins[i].caretPositioned()
        }

    },

    fireClicked: function(e)
    {
        var val = true;
        for (var i in this.plugins) {
            if (this.plugins[i].clicked(e) === false) {
                val = false;
            }
        }

        return val;

    },

    fireMouseDown: function(e)
    {
        var val = true;
        for (var i in this.plugins) {
            if (this.plugins[i].mouseDown(e) === false) {
                val = false;
            }
        }

        return val;

    },

    fireKeyDown: function(e)
    {
        var val = true;
        for (var i in this.plugins) {
            if (this.plugins[i].keyDown(e) === false) {
                val = false;
            }
        }

        return val;

    },

    fireKeyPress: function(e)
    {
        var val = true;
        for (var i in this.plugins) {
            if (this.plugins[i].keyPress(e) === false) {
                val = false;
            }
        }

        return val;

    },

    fireEnabled: function(enabled)
    {
        for (var i in this.plugins) {
            this.plugins[i].setEnabled(enabled);
        }

    },

    fireSaving: function(elem)
    {
        for (var i in this.plugins) {
            if (this.plugins[i].saving) {
                this.plugins[i].saving(elem);
            }
        }

    },

    fireCaretUpdated: function()
    {
        for (var i in this.plugins) {
            if (this.plugins[i].caretUpdated) {
                this.plugins[i].caretUpdated();
            }
        }

    },

    registerCallback: function(type, id, callback)
    {
        if (dfx.isFn(callback) === false) {
            return;
        }

        if (dfx.isset(this.callbacks[type]) === false) {
            this.callbacks[type] = {};
        }

        this.callbacks[type][id] = callback;

    },

    fireCallbacks: function(type, data)
    {
        if (dfx.isset(this.callbacks[type]) === true) {
            for (var id in this.callbacks[type]) {
                this.callbacks[type][id].call(type, data);
            }
        }

    },

    removeCallback: function(type, id)
    {
        delete this.callbacks[type][id];

    }

};

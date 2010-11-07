/**
 * JS Class for the Text Box Widget.
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
 * @subpackage Role
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var RoleRoleScreen = new function()
{
    var _data             = null;
    var _prevSelectedRole = null;
    var _removedRoles     = {};

    this.initScreen = function(data) {
        _data        = data;
        removedRoles = {};

        // Add the mouse hover event for list items.
        var elems = dfx.getClass('RoleScreen-privListItem');
        dfx.hover(elems, function (e) {
            dfx.addClass(e.currentTarget, 'hover');
        }, function(e) {
            dfx.removeClass(e.currentTarget, 'hover');
        });

        // Add Click event for the list items. Click handler will determine the
        // clicked element.
        var topElem = dfx.getClass('RoleScreen-privilegeList', dfx.getId('RolesScreen'));
        dfx.addEvent(topElem, 'click', function(e) {
            _handleClick(e, topElem);
        });

        // Set the initial selected role's name.
        _prevSelectedRole = GUI.getWidget('rolesList').getSelectedItemId();
        if (_prevSelectedRole !== undefined) {
            GUI.getWidget('role-name').setValue(_data[_prevSelectedRole].name);
            GUI.setModified(GUI.getWidget('role-name'), false);
        }

        // Add event call back for SettingsBox item selection.
        var self = this;
        GUI.getWidget('rolesList').addItemSelectedCallback(function(selectedRoleid) {
            // When the item is clicked show that role on the right side.
            self.showRoleSettings(selectedRoleid);
        });

        // Add role item delete toggle event.
        GUI.getWidget('rolesList').addItemToggledCallback(function(itemid, deleted, itemElement) {
            GUI.setModified('Role:RoleScreen', true);
            // Item was deleted or re-added.
            if (deleted === true) {
                removedRoles[itemid] = true;
            } else if (removedRoles[itemid]) {
                delete removedRoles[itemid];
            }
        });

        // Unrestricted access toggle button change event.
        // Also set the initial values.
        var toggleButtons = dfx.getClass('GUIToggleButton', dfx.getClass('RoleScreen-unrestrictedToggle'));
        dfx.foreach(toggleButtons, function(idx) {
            var buttonElement = toggleButtons[idx];
            var toggleWidget  = GUI.getWidget(buttonElement.id);
            if (!toggleWidget) {
                return;
            }

            toggleWidget.addToggleOnCallback(function() {
                self.setAllPrivilegesState(true);
            });

            toggleWidget.addToggleOffCallback(function() {
                self.setAllPrivilegesState(false);
            });
        });
    };

    this.showRoleSettings = function(roleid) {
        if (!roleid || !_data[roleid]) {
            return;
        }

        // Save the previously selected role settings.
        _saveRoleSettings(_prevSelectedRole);

        var role = _data[roleid];

        // Change the role name field.
        GUI.getWidget('role-name').setValue(role.name);
        GUI.setModified(GUI.getWidget('role-name'), false);

        // Set the toggle button values.
        var toggleButtons = dfx.getClass('GUIToggleButton', dfx.getClass('RoleScreen-unrestrictedToggle'));
        dfx.foreach(toggleButtons, function(idx) {
            var buttonElement = toggleButtons[idx];
            var toggleWidget  = GUI.getWidget(buttonElement.id);
            if (!toggleWidget) {
                return;
            }

           var privid = toggleWidget.id.split('-')[0];
           if (role.unrestricted && role.unrestricted.inArray(privid) === true) {
               toggleWidget.setValue(true, true);
           } else {
               toggleWidget.setValue(false, true);
           }

           GUI.setModified(toggleWidget, false);
        });

        // Update the privilege settings on the screen using roles granted list.
        _updatePrivilegeStates(role.granted);

        _prevSelectedRole = roleid;
    };

    var _saveRoleSettings = function(roleid) {
        if (!_data[roleid]) {
            return;
        }

        // Get the role name.
        _data[roleid].name = GUI.getWidget('role-name').getValue();

        // Unrestricted access setting.
        var toggleButtons = dfx.getClass('GUIToggleButton main', dfx.getId('RolesScreen'));
        _data[roleid].unrestricted = [];
        dfx.foreach(toggleButtons, function(i) {
            var id    = toggleButtons[i].id;
            var value = GUI.getWidget(id).getValue();
            if (value === true) {
                _data[roleid].unrestricted.push(id.split('-')[0]);
            }
        });

        var granted = [];
        dfx.foreach(_data[roleid].unrestricted, function(i) {
            // Each unrestricted privilege needs to be added to granted array, so that
            // later on we do not add its child privileges to 'granted' array.
            var priv = _data[roleid].unrestricted[i] + '.%';
            granted.push(priv);
        });

        // Get the granted settings.
        var items = dfx.getClass('RoleScreen-privListItem', dfx.getId('RolesScreen'));
        var iln   = items.length;

        for (var i = 0; i < iln; i++) {
            var item = items[i];
            if (granted.inArray(item.id.split('.')[0] + '.%') === true) {
                // No need to add this privilege to granted array since its top level
                // privilege has unrestricted access.
                continue;
            }

            if (dfx.hasClass(item, 'enabled') === true
                || dfx.hasClass(item, 'enabledByParent') === true
            ) {
                if (dfx.hasClass(item, 'expand') === true
                    || dfx.hasClass(item, 'collapse') === true
                ) {
                    granted.push(item.id + '.%');
                } else {
                    granted.push(item.id);
                }
            }
        }//end for

        _data[roleid].granted = granted;
    };

    var _updatePrivilegeStates = function(granted) {
        var items = dfx.getClass('RoleScreen-privListItem', dfx.getId('RolesScreen'));
        var iln   = items.length;
        var gln   = granted.length;

        for (var i = 0; i < iln; i++) {
            var privilege = items[i].id;
            var selected  = false;
            for (var j = 0; j < gln; j++) {
                var grantedPriv = granted[j];
                if (privilege === grantedPriv || privilege + '.%' === grantedPriv) {
                    selected = true;
                    break;
                } else if (privilege.indexOf(grantedPriv.replace('%', '')) === 0) {
                    selected = 'sel-inherit';
                    break;
                }
            }

            dfx.removeClass(items[i], 'enabled');
            dfx.removeClass(items[i], 'enabledByParent');
            if (selected === true) {
                dfx.addClass(items[i], 'enabled');
            } else if (selected === 'sel-inherit') {
                dfx.addClass(items[i], 'enabledByParent');
            }
        }//end for
    };

    var _handleClick = function(e, topElem) {
        var target = dfx.getMouseEventTarget(e);
        if (dfx.hasClass(target, 'RoleScreen-privListItem-toggle') === true) {
            GUI.setModified('Role:RoleScreen', true);
            if (changeToggleState(target.parentNode) === true) {
                return;
            }
        } else {
            if (toggleExpandCollapse(target, topElem) === true) {
                return;
            }
        }
    };

    var toggleExpandCollapse = function(target, topElem) {
        var elem = target;
        if (dfx.hasClass(target, 'RoleScreen-privListItem') === false) {
            var parents = dfx.getParents(target, 'div.RoleScreen-privListItem', topElem);
            if (parents.length > 0) {
                elem = parents[0];
            }
        }

        if (dfx.hasClass(elem, 'expand') === true) {
            dfx.swapClass(elem, 'expand', 'collapse');
            toggleChildPrivileges(elem, 'hide');
            return true;
        } else if (dfx.hasClass(elem, 'collapse') === true) {
            dfx.swapClass(elem, 'collapse', 'expand');
            toggleChildPrivileges(elem, 'show');
            return true;
        }

        return false;
    };

    var toggleChildPrivileges = function(elem, state) {
        var parentid = elem.id + '.';
        for (var node = elem.nextSibling; node; node = node.nextSibling) {
            if (node.nodeType !== 1) {
                continue;
            } else if (node.id.indexOf(parentid) === 0) {
                if (state === 'hide') {
                    dfx.hideElement(node);
                } else {
                    dfx.showElement(node);
                }
            } else {
                break;
            }
        }
    };

    var changeToggleState = function(toggle, forceState, unrestricted) {
        if (dfx.isset(forceState) === false && dfx.hasClass(toggle, 'enabledByParent') === true) {
            return;
        }

        var className = 'enabled';
        if (unrestricted === true) {
            className = 'enabledByParent';
            dfx.removeClass(toggle, 'enabled');
        }

        if (dfx.hasClass(toggle, 'expand') === true
                || dfx.hasClass(toggle, 'collapse') === true
            ) {
            if (forceState === true) {
                dfx.addClass(toggle, className);
                changeChildPrivilegeStates(toggle, true);
            } else if (forceState === false) {
                dfx.removeClass(toggle, className);
                changeChildPrivilegeStates(toggle, false);
            } else {
                // A parent so update it self and its children.
                if (dfx.hasClass(toggle, className) === true) {
                    dfx.removeClass(toggle, className);
                    changeChildPrivilegeStates(toggle, false);
                } else {
                    dfx.addClass(toggle, className);
                    changeChildPrivilegeStates(toggle, true);
                }
            }
        } else {
            if (forceState === false || (forceState !== true && dfx.hasClass(toggle, className) === true)) {
                dfx.removeClass(toggle, className);
            } else {
                dfx.addClass(toggle, className);
            }
        }//end if
    };

    var changeChildPrivilegeStates = function(elem, state) {
        var parentid = elem.id + '.';
        for (var node = elem.nextSibling; node; node = node.nextSibling) {
            if (node.nodeType !== 1) {
                continue;
            } else if (node.id.indexOf(parentid) === 0) {
                dfx.removeClass(node, 'enabled');
                if (state === true) {
                    dfx.addClass(node, 'enabledByParent');
                } else {
                    dfx.removeClass(node, 'enabledByParent');
                }
            } else {
                break;
            }
        }
    };

    this.setAllPrivilegesState = function(state) {
        // Get the first element of the currently selected privilege type (e.g. asset).
        var visibleElement = GUI.getWidget('role-switcher').getVisibleContentElement();
        if (!visibleElement) {
            return;
        }

        var privListElements = dfx.getClass('RoleScreen-privListItem level-1', visibleElement);
        dfx.foreach(privListElements, function(i) {
            changeToggleState(privListElements[i], state, true);
        });
    };

    this.createNewRole = function() {
        var itemid = 'new_' + dfx.getUniqueId();
        var title  = 'New Role';

        if (_data instanceof Array) {
            // This could happen if this is the first role.
            _data = {};
        }

        // Init the item info.
        _data[itemid] = {
            granted: {},
            name: title
        };

        GUI.getWidget('rolesList').addItem(itemid, title, true);

        // Focus textbox so that user can type in the new name.
        GUI.getWidget('role-name').select();

        // Remove any "no roles" class on the content.
        dfx.addClass(dfx.getClass('RolesScreenMainContent'), 'has-roles');
        dfx.removeClass(dfx.getClass('RolesScreenMainContent'), 'no-roles');

        GUI.setModified('Role:RoleScreen', true);
    };

    this.getValue = function() {
        // Before we return value need to get the current selected items updated values.
        _saveRoleSettings(GUI.getWidget('rolesList').getSelectedItemId());
        var data = {
            roles: _data,
            removedRoles: removedRoles
        };

        return data;
    };

    this.saved = function() {
        GUI.setModified('Role:RoleScreen', false);
        GUI.reloadTemplate('Role:RoleScreen');
    };

};

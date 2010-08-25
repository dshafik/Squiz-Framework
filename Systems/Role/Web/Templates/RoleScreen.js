var RoleScreen = new function()
{
    var _data             = null;
    var _prevSelectedRole = null;

    this.initScreen = function(data) {
        _data = data;

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
        GUI.getWidget('role-name').setValue(_data[_prevSelectedRole].name);

        // Add event call back for SettingsBox item selection.
        var self = this;
        GUI.getWidget('rolesList').addItemSelectedCallback(function(selectedRoleid) {
            // When the item is clicked show that role on the right side.
            self.showRoleSettings(selectedRoleid);
        });

        // Unrestricted access toggle button change event.
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

        // Update the privilege settings on the screen using roles granted list.
        _updatePrivilegeStates(role.granted);

        _prevSelectedRole = roleid;
    };

    var _saveRoleSettings = function(roleid) {
        // Get the role name.
        _data[roleid].name = GUI.getWidget('role-name').getValue(name);

        // Get the granted settings.
        var items = dfx.getClass('RoleScreen-privListItem', dfx.getId('RolesScreen'));
        var iln   = items.length;

        var granted = [];
        for (var i = 0; i < iln; i++) {
            var item = items[i];
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

    var changeToggleState = function(toggle, forceState) {
        if (dfx.isset(forceState) === false && dfx.hasClass(toggle, 'enabledByParent') === true) {
            return;
        }

        GUI.setModified(this, true);
        if (dfx.hasClass(toggle, 'expand') === true
                || dfx.hasClass(toggle, 'collapse') === true
            ) {
            if (forceState === true) {
                dfx.addClass(toggle, 'enabled');
                changeChildPrivilegeStates(toggle, true);
            } else if (forceState === false) {
                dfx.removeClass(toggle, 'enabled');
                changeChildPrivilegeStates(toggle, false);
            } else {
                // A parent so update it self and its children.
                if (dfx.hasClass(toggle, 'enabled') === true) {
                    dfx.removeClass(toggle, 'enabled');
                    changeChildPrivilegeStates(toggle, false);
                } else {
                    dfx.addClass(toggle, 'enabled');
                    changeChildPrivilegeStates(toggle, true);
                }
            }
        } else {
            if (forceState === false || dfx.hasClass(toggle, 'enabled') === true) {
                dfx.removeClass(toggle, 'enabled');
            } else {
                dfx.addClass(toggle, 'enabled');
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
            changeToggleState(privListElements[i], state);
        });
    };

    this.createNewRole = function() {
        GUI.getWidget('role-switcher').createNewItem(function() {
            console.info(1);
        });
    };

    this.getValue = function() {
        // Before we return value need to get the current selected items updated values.
        _saveRoleSettings(GUI.getWidget('rolesList').getSelectedItemId());
        var data = {
            roles: _data
        };

        return data;
    };

};

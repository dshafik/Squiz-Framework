var RoleScreen = new function()
{
    var _data             = null;
    var _prevSelectedRole = null;
    this.initScreen = function(data)
    {
        _data  = data;
        var elems = dfx.getClass('RoleScreen-privListItem');
        dfx.hover(elems, function (e) {
            dfx.addClass(e.currentTarget, 'hover');
        }, function(e) {
            dfx.removeClass(e.currentTarget, 'hover');
        });

        // Add Click event.
        var topElem = dfx.getClass('RoleScreen-privilegeList', dfx.getId('RolesScreen'));
        dfx.addEvent(topElem, 'click', function(e) {
            _handleClick(e, topElem);
        });

        _prevSelectedRole = GUI.getWidget('rolesList').getSelectedItemId();
        GUI.getWidget('role-name').setValue(_data[_prevSelectedRole].name);

        // Add event call back for Settings box item selection.
        var self = this;
        GUI.getWidget('rolesList').addItemSelectedCallback(function(selectedRoleid) {
            self.showRoleSettings(selectedRoleid);
        });

    };

    this.showRoleSettings = function(roleid)
    {
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

    var _saveRoleSettings = function(roleid)
    {
        // Get the role name.
        _data[roleid].name = GUI.getWidget('role-name').getValue(name);

        // get the granted settings.
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

    var _updatePrivilegeStates = function(granted)
    {
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
        }


    };

    var _handleClick = function(e, topElem)
    {
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

    var toggleExpandCollapse = function(target, topElem)
    {
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

    var toggleChildPrivileges = function(elem, state)
    {
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

    var changeToggleState = function(toggle)
    {
        if (dfx.hasClass(toggle, 'enabledByParent') === true) {
            return;
        }

        GUI.setModified(this, true);
        if (dfx.hasClass(toggle, 'expand') === true
                || dfx.hasClass(toggle, 'collapse') === true
            ) {
            // A parent so update its children.
            if (dfx.hasClass(toggle, 'enabled') === true) {
                dfx.removeClass(toggle, 'enabled');
                changeChildPrivilegeStates(toggle, false);
            } else {
                dfx.addClass(toggle, 'enabled');
                changeChildPrivilegeStates(toggle, true);
            }
        } else {
            if (dfx.hasClass(toggle, 'enabled') === true) {
                dfx.removeClass(toggle, 'enabled');
            } else {
                dfx.addClass(toggle, 'enabled');
            }
        }

    };

    var changeChildPrivilegeStates = function(elem, state)
    {
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


    this.createNewRole = function()
    {
        console.info('add new');

    };

    this.getValue = function()
    {
        // Before we return value need to get the current selected items updated values.
        _saveRoleSettings(GUI.getWidget('rolesList').getSelectedItemId());
        var data = {
            roles: _data
        };

        return data;

    };


};

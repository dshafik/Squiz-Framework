var RoleScreen = new function()
{
    this.initScreen = function()
    {
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

    };

    var _handleClick = function(e, topElem)
    {
        var target = dfx.getMouseEventTarget(e);
        if (dfx.hasClass(target, 'RoleScreen-privListItem-toggle') === true) {
            if (changeToggleState(target) === true) {
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
        console.info(toggle);
    };

    this.createNewRole = function()
    {
        console.info('add new');

    };


};

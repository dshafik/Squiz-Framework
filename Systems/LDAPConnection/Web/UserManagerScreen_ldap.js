/**
 * JS Class for the LDAP UserManager Screen.
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
 * @subpackage LDAPConnection
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var UserUserManagerScreen_ldap = new function()
{
    var _fields = {
        details: {
            host: null,
            port: null,
            baseDN: null,
            bindDN: null,
            password: null,
            authDN: null,
            authFilter: null
        },
        userAttributes: {
            uid: null,
            cn: null,
            givenname: null,
            sn: null,
            mail: null
        },
        groupAttributes: {
            ou: null,
            groupMembership: null,
            groupMembers: null
        },
        uniqueIdSettings: {
            uniqueIdUsers: null,
            uniqueIdGroups: null
        }
    };

    this.createConnection = function() {
        var browserWidget = GUI.getWidget('userManager-assetBrowser');
        var details       = _getValue('LDAPConnection', true);

        GUI.sendRequest('LDAPConnection', 'createConnection', details, function(response) {
            if (!response.match(/^\d+$/)) {
                //GUI.queueOverlay({
                //    icon: 'warning',
                //    content: [response]
                //});
            } else {
                UserUserManagerScreen.reloadBrowser();
            }
        }, 'raw');
    };

    this.getValue = function(assetType) {
        return _getValue(assetType);
    };

    var _getValue = function(assetType,jsonEncode) {
        var browserWidget = GUI.getWidget('userManager-assetBrowser');
        var idPrefix      = 'LDAPUserManagerScreen-new';

        var details = {
        };

        switch (assetType) {
            case 'LDAPUser':
            case 'LDAPUserGroup':
                details.assetid = dfx.jsonEncode((browserWidget.getValue()).pop());
                details.parents = GUI.getWidget('UserManagerScreen-parentsList').getValue();
            break;

            case 'LDAPConnection':
                details.name = GUI.getWidget(idPrefix + '-name').getValue();
                details.parentAssetid = dfx.jsonEncode((browserWidget.getValue()).pop());

                dfx.foreach(_fields, function(groupName) {
                    if (!details[groupName]) {
                        details[groupName] = {};
                    }

                    dfx.foreach(_fields[groupName], function(fieldid) {
                        details[groupName][fieldid] = GUI.getWidget(idPrefix + '-' + fieldid).getValue();
                    });

                    if (jsonEncode === true) {
                        details[groupName] = dfx.jsonEncode(details[groupName]);
                    }
                });
            break;

        };

        return details;
    };

};

/**
 * JS Class for the Viper Plugin.
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

function ViperPlugin(viper)
{
    this.viper = viper;

}

ViperPlugin.ALL_EVENTS        = 0;
ViperPlugin.NODES_DELETED     = 1;
ViperPlugin.NODES_INSERTED    = 2;
ViperPlugin.NODES_CHANGED     = 3;
ViperPlugin.SELECTION_CHANGED = 4;

ViperPlugin.prototype = {

    start: function() {},
    clicked: function(e)
    {
        return true;

    },
    mouseDown: function(e)
    {
        return true;

    },
    keyDown: function(e)
    {
        return true;

    },
    keyPress: function(e)
    {
        return true;

    },
    selectionChanged: function(range) {},
    setEnabled: function(enabled) {},
    saving: function() {},
    caretUpdated: function() {},
    nodeInserted: function(node, range) {},
    caretPositioned: function() {},
    remove: function()
    {
        ViperPluginManager.removeKeyPressListener(this);

    },
    setSettings: function(settings) {}
};

dfx.noInclusionInherits('ViperPlugin', 'AbstractWidgetWidgetType', true);
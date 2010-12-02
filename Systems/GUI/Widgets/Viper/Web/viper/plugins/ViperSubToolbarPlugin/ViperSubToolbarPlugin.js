/**
 * Displays a sub toolbar under the main Viper toolbar.
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

function ViperSubToolbarPlugin(viper)
{
    ViperPlugin.call(this, viper);

    this._toolbars    = {};
    this._wrapper     = null;
    this._activeBarid = null;

}

ViperSubToolbarPlugin.prototype = {

    createToolBar: function(id)
    {
        if (this._wrapper === null) {
            this._wrapper = this._createWrapper();
        }

        var barid  = 'subToolbar-' + id;
        var oldbar = dfx.getId(barid);
        if (oldbar) {
            dfx.remove(oldbar);
        }

        var c   = 'ViperSubToolbar';
        var div = document.createElement('div');
        div.id  = barid;
        dfx.addClass(div, c);
        this._toolbars[id] = div;

        // Get toolbars position.
        var toolbarEl = dfx.getId('ViperToolbar');
        if (toolbarEl && toolbarEl.parentNode !== document.body) {
            this.setParentElement(toolbarEl.parentNode);
        }

        var content = '<div class="' + c + '-left"></div>';
        content    += '<div class="' + c + '-mid"><div class="' + c + '-mid-wrapper"></div></div>';
        content    += '<div class="' + c + '-right"></div>';
        dfx.setHtml(div, content);

        var mid = dfx.getClass(c + '-mid-wrapper', div)[0];

        this._wrapper.appendChild(div);

        return mid;

    },


    setParentElement: function(parent)
    {
        dfx.remove(this._wrapper);
        dfx.setStyle(this._wrapper, 'position', 'absolute');
        dfx.setStyle(this._wrapper, 'top', '22px');
        parent.appendChild(this._wrapper);

    },

    _createWrapper: function()
    {
        var wrapper = document.createElement('div');
        dfx.addClass(wrapper, 'ViperSubToolbar-wrapper');
        document.body.appendChild(wrapper);

        return wrapper;

    },

    showToolbar: function(id)
    {
        var bar = this.getBar(id);
        if (bar) {
            if (this._activeBarid && id !== this._activeBarid) {
                this.hideToolbar(this._activeBarid);
            }

            dfx.setStyle(bar, 'visibility', 'hidden');
            dfx.addClass(bar, 'active');

            // Next line is important, if the bar is long and browser
            // window size is small then bar contents may wrap to the next line
            // giving incorrect width value.
            dfx.setStyle(bar.parentNode, 'right', 0);
            var width = dfx.getElementWidth(bar);
            dfx.setStyle(bar, 'margin-right', ((width * -1) / 2) + 'px');
            dfx.setStyle(bar.parentNode, 'right', '50%');
            dfx.setStyle(bar, 'visibility', 'visible');
        }

        this.viper.fireCallbacks('ViperSubToolbar:showToolbar', id);

        this._activeBarid = id;

    },

    hideToolbar: function(id)
    {
        if (this._activeBarid === id) {
            dfx.removeClass(dfx.getClass('ViperSubToolbar', this._wrapper), 'active');
            this._activeBarid = null;
            this.viper.fireCallbacks('ViperSubToolbar:hideToolbar', id);
        }

    },

    toggleToolbar: function(id)
    {
        if (this._activeBarid === id) {
            this.hideToolbar(id);
            return false;
        } else {
            this.showToolbar(id);
            return true;
        }

    },

    createOptionsList: function(title)
    {
        var div = document.createElement('div');
        var c   = 'ViperSubToolbar-optsList';
        dfx.addClass(div, c);

        var contents = '<div class="' + c + '-title">';
        contents    += '<div class="' + c + '-title-left"></div>';
        contents    += '<div class="' + c + '-title-mid">' + title + '</div>';
        contents    += '<div class="' + c + '-title-right"></div></div>';
        contents    += '<div class="' + c + '-mid"></div>';
        contents    += '<div class="' + c + '-right"></div>';
        dfx.setHtml(div, contents);

        var contentDiv = dfx.getClass(c + '-mid', div)[0];

        return {
            main: div,
            contentEl: contentDiv
        };

    },

    getBar: function(id)
    {
        return this._toolbars[id];

    },

    isPluginElement: function(el)
    {
        return dfx.isChildOf(el, this._wrapper);

    },

    remove: function()
    {
        dfx.remove(this._wrapper);

    },

    isActive: function()
    {
        if (this._activeBarid) {
            return true;
        }

        return false;

    }

};

dfx.noInclusionInherits('ViperSubToolbarPlugin', 'ViperPlugin', true);

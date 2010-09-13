/**
 * JS Class for Help system.
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
 * @subpackage Help
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var Help = new function()
{
    var _apiURL = null;

    this.init = function() {
        // Set the API URL.
        _apiURL = '/__api/raw/Help/getPage?_api_token=' + dfx.getId('__api_token').value + '&';

        var elem = dfx.getId('Help-dialog');

        var navHeight = parseInt(dfx.getElementHeight(dfx.getClass('Help-controls', elem)[0]));
        var iframe    = dfx.getId('Help-iframe');
        var self      = this;
        iframe.onload = function(e) {
            self.iframeLoaded();
        }

        var midElem    = dfx.getClass('GUIDialog-middle', elem)[0];
        var initHeight = parseInt(dfx.getElementHeight(midElem));

        dfx.setStyle(iframe, 'height', (initHeight - navHeight - 27) + 'px');

        GUI.getWidget('Help-dialog').addDialogResizedCallback(function(ui, contentSize) {
            dfx.setStyle(iframe, 'height', (contentSize.height - navHeight - 27) + 'px');
        });
    }

    this.refresh = function() {
        this.loadIndexPage();
    };

    this.loadGlossaryPage = function(template) {
        template = template || GUI.getCurrentTemplate();

        // Construct the glossary page id for this template.
        var pageid = template.split(':').join('-') + '-glossary-index.html';
        this.loadPage(pageid);
    };

    this.loadIndexPage = function(template) {
        template   = template || GUI.getCurrentTemplate();
        var pageid = template.split(':').join('-') + '-index.html';
        this.loadPage(pageid);
    };

    this.loadGeneralPage = function() {
        this.loadPage('general.html');
    };

    this.loadPage = function(pageid) {
        var helpIframe = dfx.getId('Help-iframe');

        dfx.attr(helpIframe, 'src', _apiURL + 'pageid=' + escape(pageid));
    };

    this.iframeLoaded = function() {
        jQuery(dfx.getId('Help-iframe')).localScroll({offset: {top: -32}});
    };

    this.back = function() {
        history.back(1);
    };

    this.forward = function() {
        history.forward(1);
    };

    this.home = function() {
        this.loadIndexPage();
    };

    this.glossary = function() {
        this.loadGlossaryPage();
    };

    this.general = function() {
        this.loadGeneralPage();
    };

};

/**
 * Collection of AJAX related JS functions.
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

if (!window.dfx) {
    window.dfx = function() {};
}

dfx.get = function(url, data, callBack)
{
    url = dfx.cleanAjaxRequestUrl(url);
    jQuery.get(url, data, callBack);

};


dfx.post = function(url, data, successCallback, errorCallback, timeout)
{
    url = dfx.cleanAjaxRequestUrl(url);
    timeout = timeout || 20;
    jQuery.ajax({
        url: url,
        type: 'POST',
        data: data,
        success: successCallback,
        error: errorCallback,
        timeout: (timeout * 1000)
    });

};

/**
 * Retrieves JSON encoded data from the URL.
 *
 * Note: make sure you have parenthesis around your json string else
 * JS will throw "invalid label" error.
 */
dfx.getJSON = function(url, data, callBack)
{
    url = dfx.cleanAjaxRequestUrl(url);
    jQuery.getJSON(url, data, callBack);

};

/**
 * Returns a cleaned URL to make an ajax request with.
 */
dfx.cleanAjaxRequestUrl = function(url)
{
    if (typeof url !== 'string') {
        // Do the jquery URL default to current location on undefined
        // URL earlier so we can catch the IE8 hash tag bug
        var url = location.href;
    }

    // No ajax requests should have an anchor part in the URL.
    // Also IE8 bug - Anchor becomes part of last query string value.
    url = dfx.noAnchorPartUrl(url);

    return url;
};

dfx.toQueryStr = function(params)
{
    return jQuery.param(params);

};

dfx.addToQueryStr = function(url, params)
{
    if (typeof params !== "string") {
        params = dfx.toQueryStr(params);
    }

    var m = url.match(/\?/);
    if (dfx.isset(m) === true && m.length > 0) {
        url += '&';
    } else {
        url += '?';
    }

    url += params;

    return url;

};

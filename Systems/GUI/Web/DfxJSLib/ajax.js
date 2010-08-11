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
    jQuery.get(url, data, callBack);

};


dfx.post = function(url, data, successCallback, errorCallback, timeout)
{
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
    jQuery.getJSON(url, data, callBack);

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

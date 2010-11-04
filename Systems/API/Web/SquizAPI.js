/**
 * Provides an API to Squiz Suite products.
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
 * @subpackage API
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Provides namespacing for Squiz Framework API (sfapi)
 */
var sfapi = {};

/**
 * Root URL to send the request.
 * @type String
 */
sfapi.rootUrl = '';

/**
 * API URL suffix.
 * @type String
 */
sfapi.rootUrlSuffix = '__api';

/**
 * ID for script tag to use.
 * @type String
 */
sfapi.scriptTagid = '__sfapi';

/**
 * Callback function when the script is loaded.
 * @type Function
 */
sfapi._getCallback = null;

/**
 * Session token that being submitted in every request.
 * @type String
 */
sfapi._api_token = null;

/**
 * For each related video in the JSON feed, add an thumbnail representing
 * that video to the div specified by a constant.
 * @param {String} Name of System to make action call.
 * @param {String} Name of Channel action to call.
 * @param {Object} Parameters to pass via GET reuqest.
 * @param {Function} callback Optional callback function.
 * @param {String} format The response format.
 */
sfapi.get = function(system, action, params, callback, format)
{
    // Create script tag if it hasn't been created.
    var scriptTag = document.getElementById(sfapi.scriptTagid);
    var head      = document.getElementsByTagName("head").item(0);

    // TODO: Do we need to ever need to repeat an API call?
    // If we do, then there is a problem if the same script tag is used - a lack of
    // change in the script tag's src means it wouldn't run again.
    if (scriptTag) {
        head.removeChild(scriptTag);
    }

    scriptTag = document.createElement('script');
    scriptTag.setAttribute('id', sfapi.scriptTagid);
    scriptTag.setAttribute('type', 'text/javascript');
    head.appendChild(scriptTag);

    // Form URL to send the request.
    var src = sfapi.createURL(system, action, format, params);
    src    += '?_callback=sfapi._getCallback';

    src = sfapi.attachTokenToURL(src);

    sfapi._getCallback = function(data) {
        if (data.error) {
            alert('Invalid token');
        } else {
            if (callback) {
                callback.call(null, data);
            }
        }
    };

    // Do it!
    scriptTag.src = src;

};

sfapi.post = function(system, action, params, successCallback, errorCallback, format)
{
    var url = sfapi.createURL(system, action, format);

    var token = document.getElementById('__api_token');
    if (token) {
        params._api_token = token.value;
    }

    if (!errorCallback) {
        errorCallback = function(xhr) {
            if (console && console.error && xhr && xhr.responseText) {
                console.error(xhr.statusText);
            }
        };
    }

    dfx.post(url, params, function(data) {
        if (data.error) {
            alert('Invalid token');
        } else if (successCallback) {
            successCallback.call(null, data);
        }
    }, errorCallback);

};

sfapi.createURL = function(system, action, format, params)
{
    format = format || 'json';

    var queryString   = '';
    var queryStrIndex = sfapi.rootUrl.indexOf('?');
    if (queryStrIndex >= 0) {
        queryString   = sfapi.rootUrl.substr(queryStrIndex);
        sfapi.rootUrl = sfapi.rootUrl .substr(0, queryStrIndex);
    }

    var url = sfapi.rootUrl + '/' + sfapi.rootUrlSuffix;
    url    += '/' + format + '/' + system + '/' + action + queryString;

    return url;

};

sfapi.attachTokenToURL = function(url)
{
    var token = document.getElementById('__api_token');
    if (token) {
        var tokenParam = {
            _api_token: token.value
        };

        url = dfx.addToQueryString(url, tokenParam);
    }

    return url;

};
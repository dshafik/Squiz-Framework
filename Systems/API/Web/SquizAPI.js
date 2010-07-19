/**
 * provides namespacing for Squiz Framework API (sfapi)
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
 * Return data format.
 * @type String
 */
sfapi.outputDataFormat = 'json';


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
 */
sfapi.get = function(system, action, params, callback) {
    // Create script tag if it hasn't been created.
    var scriptTag = document.getElementById(sfapi.scriptTagid);
    var head = document.getElementsByTagName("head").item(0);

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
    var src = sfapi.rootUrl + '/' + sfapi.rootUrlSuffix + '/' + sfapi.outputDataFormat;
    src    += '/' + system + '/' + action;;
    src    += '?_callback=sfapi._getCallback&_api_token=' + sfapi._api_token;

    if (params) {
        for (var id in params) {
            if (params.hasOwnProperty(id) === true) {
                src += '&' + id + '=' + escape(params[id])
            }
        }
    }

    sfapi._getCallback = function(data) {
        if (data.new_token) {
            // NULL token must been submitted.
            sfapi._api_token = data.new_token;
            sfapi.get(system, action, params, callback);
        } else {
            if (data.next_token) {
                // A new token for the next request came.
                sfapi._api_token = data.next_token;
            }

            if (callback) {
                callback.call(data);
            }
        }
    };

    // Do it!
    scriptTag.src = src;

};
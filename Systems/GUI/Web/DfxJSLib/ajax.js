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

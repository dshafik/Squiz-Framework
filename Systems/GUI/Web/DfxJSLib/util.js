/**
 * The util package.
 */
var Util = {};

/**
 * A hash table indexed by an object.
 *
 * @constructor
 */
Util.ObjectHash = function()
{
    /*
     * The array of objects.
     *
     * @var array
     */
    this.objects = [];

    /*
     * The array of values.
     *
     * Each indice in this array corresponds the owning object for that value.
     *
     * @var array
     */
    this.values = [];

};


/*
 * Returns the index used to index this object.
 *
 * @param {Object} object The object to obtain the index for.
 *
 * @return integer
 */
Util.ObjectHash.prototype.getObjectIndex = function(object)
{
    var oln = this.objects.length;
    for (var i = 0; i < oln; i++) {
        if (this.objects[i] === object) {
           return i;
        }
    }

    return -1;

};


/*
 * Puts the specified value in the hash indexed by the specified object.
 *
 * If the object already exists in the hash, the value will be replaced with the
 * new value.
 *
 * @param {Object} object The object that represents the key.
 * @param {Mixed}  value  The value to store.
 *
 * @return void
 */
Util.ObjectHash.prototype.put = function(object, value)
{
    var index = this.getObjectIndex(object);
    if (index !== -1) {
        this.values[index] = value;
    } else {
        this.objects.push(object);
        this.values.push(value);
    }

};


/*
 * Returns the value for the specified object.
 *
 * @param {Object} object The object that represents the key.
 *
 * @return mixed or null if the object does not exist in the hash.
 */
Util.ObjectHash.prototype.get = function(object)
{
    var index = this.getObjectIndex(object);

    if (index !== -1) {
        return this.values[index];
    }

    return null;

};


/*
 * Returns TRUE if the specified object exists in the hash.
 *
 * @param {Object} object The object that represents the key.
 *
 * @return boolean
 */
Util.ObjectHash.prototype.containsKey = function(object)
{
    return (this.getObjectIndex(object) != -1);

};


/*
 * Replaces the value currently stored for the specified object.
 *
 * @param {Object} object The object that represents the key.
 * @param {Mixed}  value  The new value to store.
 *
 * @return void
 */
Util.ObjectHash.prototype.replace = function(object, value)
{
    var index = this.getObjectIndex(object);

    if (index !== -1) {
        this.values[index] = value;
    }

};

/**
 * Initialises the XML for parsing.
 *
 * @param string xml The XML contents.
 *
 * @return void
 */
Util.Xml = function(xml)
{
    // The xml argument is optional.
    this.xml = xml;

};

/*
 * Return an XML Object that is valid for the current browser.
 *
 * @return object
 */
Util.Xml.prototype.parse = function()
{
    var xml = this.xml;
    var doc = null;
    if (window.ActiveXObject) {
        doc       = new ActiveXObject("Microsoft.XMLDOM");
        doc.async = "false";
        doc.loadXML(xml);
    } else {
        var parser = new DOMParser();
        doc        = parser.parseFromString(xml,"text/xml");
    }

    return doc;

};

Util.Xml.getElementById = function(id, parent)
{
    if (parent) {
        if (parent.getElementById) {
            return parent.getElementById(id);
        }

        var pcln = parent.childNodes.length;
        for (var i = 0; i < pcln; i++) {
            if (parent.childNodes[i].getAttribute('id') === id) {
                return parent.childNodes[i];
            } else {
                if (parent.childNodes[i].childNodes.length > 0) {
                    var el = this.getElementById(id, parent.childNodes[i]);
                    if (el && el.getAttribute('id') === id) {
                        return el;
                    }
                }
            }
        }
    }

    return null;

};

/**
 * returns a left trimmed string.
 *
 * @param {String} value The string to trim.
 *
 * @return {String}
 */
dfx.ltrim = function(value)
{
    var re = /\s*((\S+\s*)*)/;
    return value.replace(re, "$1");

};

/**
 * returns a right trimmed string.
 *
 * @param {String} value The string to trim.
 *
 * @return {String}
 */
dfx.rtrim = function(value)
{
    var re = /((\s*\S+)*)\s*/;
    return value.replace(re, "$1");

};


/**
 * returns a trimmed string.
 *
 * @param {String} value The string to trim.
 *
 * @return {String}
 */
dfx.trim = function(value)
{
    return dfx.ltrim(dfx.rtrim(value));

};

/**
 * returns true if specified string is blank.
 *
 * @param {String} value The string to test.
 *
 * @return {String}
 */
dfx.isBlank = function(value)
{
    if (!value || /^\s*$/.test(value)) {
        return true;
    }

    return false;

};

/**
 * returns an ellipsized string.
 *
 * @param {String} value The string to trim.
 *
 * @return {String}
 */
dfx.ellipsize = function(value, length)
{
    // Type validation.
    if (typeof value !== 'string' || typeof length !== 'number') {
        return '';
    }

    // Length needs to be at least zero.
    if (length < 0) {
        return '';
    }

    // If the string is not long enough, don't change it.
    if (value.length < length) {
        return value;
    }

    value = value.substr(0, length);
    value = value.replace(/\s$/, '');

    // Figure out how many dots are on the end of the
    // string so we don't add too many.
    var end       = value.substr((length - 3), 3);
    var endNoDots = end.replace(/\.$/, '');
    var numDots   = (end.length - endNoDots.length);

    value = value + dfx.strRepeat('.', (3 - numDots));
    return value;

};


dfx.ellipsizeDom = function(elem, length)
{
    var browserInfo = dfx.browser();
    if (browserInfo.type === 'msie') {
        // Handle ellipsis with CSS style, text-align: ellipsis;.
        dfx.setStyle(elem, 'text-overflow', 'ellipsis');;
        dfx.setStyle(elem, 'white-space', 'nowrap');
        dfx.setStyle(elem, 'width', length + 'px');
    } else {
        // We have to manually handle FF browsers to correctly
        // ellipsize based on the width.
        dfx.setStyle(elem, 'visibility', 'hidden');
        var currWidth = dfx.getStyle(elem, 'width');
        currWidth     = parseInt(currWidth.substr(0, (currWidth.length - 2)), 10);
        if (currWidth > length) {
            var oriName = dfx.getHtml(elem);
            var tmpName = oriName;
            while (currWidth > length) {
                tmpName = tmpName.substring(0, (tmpName.length - 1));
                dfx.setHtml(elem, tmpName);
                currWidth = dfx.getStyle(elem, 'width');
                currWidth = parseInt(currWidth.substr(0, (currWidth.length - 2)), 10);
            }

            var ellipsisLen = tmpName.length + 1;
            oriName = dfx.ellipsize(oriName, (ellipsisLen - 4));
            dfx.setHtml(elem, oriName);
        }

        dfx.setStyle(elem, 'visibility', 'visible');
    }//end if

};

/**
 * Changes the first character to uppercase.
 */
dfx.strRepeat = function(input, multiplier)
{
   var result = '';
   for (var i = 0; i < multiplier; i++) {
       result += input;
   }

   return result;

};

/**
 * Changes the first character to uppercase.
 */
dfx.ucFirst = function(str)
{
    return str.substr(0,1).toUpperCase() + str.substr(1, str.length);

};

dfx.ucWords = function(str)
{
    return str.toLowerCase().replace(/\w+/g,function(s){
          return s.charAt(0).toUpperCase() + s.substr(1);
    });

};

/**
 * Returns true if specified var is a function
 */
dfx.isFn = function(f)
{
    if (typeof f === 'function') {
        return true;
    }

    return false;

};

dfx.isObj = function(v)
{
    if (v !== null && typeof v === 'object') {
        return true;
    }

    return false;

};

dfx.isset = function(v)
{
    if (typeof v !== 'undefined' && v !== null) {
        return true;
    }

    return false;

};

dfx.isArray = function(v)
{
    return jQuery.isArray(v);

};

/**
 * Returns true if the specified string contains numbers only.
 */
dfx.isNumeric = function(str)
{
    var result = str.match(/^\d+$/);
    if (result !== null) {
        return true;
    }

    return false;

};

dfx.clone = function(value, shallow)
{
    if (typeof value !== 'object') {
        return value;
    }

    if (value === null) {
        var valueClone = null;
    } else {
        var valueClone = new value.constructor();
        for (var property in value) {
            if (shallow) {
                valueClone[property] = value[property];
            }

            if (value[property] === null) {
                valueClone[property] = null;
            } else if (typeof value[property] === 'object') {
                valueClone[property] = dfx.clone(value[property], shallow);
            } else {
                valueClone[property] = value[property];
            }
        }
    }

    return valueClone;

};

// Return TRUE if two objects are NOT same.
dfx.objDiff = function(obj1, obj2)
{
    var count1 = 0;
    var count2 = 0;
    for (var p in obj1) {
        count1++;
    }

    for (var q in obj2) {
        count2++;
    }

    if (count1 !== count2) {
        return true;
    }

    for (var p in obj1) {
        if (obj2.hasOwnProperty(p) === false) {
            return true;
        }

        if (typeof obj1[p] === 'object') {
            if (dfx.objDiff(obj1[p], obj2[p])) {
                return true;
            }
        } else {
            if (obj1[p] !== obj2[p]) {
                return true;
            }
        }
    }

    return false;

};

dfx.baseUrl = function(fullUrl)
{
    var qStartIdx = fullUrl.search(/\?/);
    if (qStartIdx === -1) {
        return fullUrl;
    } else {
        var baseUrl = fullUrl.substr(0, qStartIdx);
        return baseUrl;
    }

}

/**
 * Return key value pairs from the given query string.
 */
dfx.queryString = function(url)
{
    var result    = {};
    var qStartIdx = url.search(/\?/);
    if (qStartIdx === -1) {
        return result;
    } else {
        var queryStr = url.substr(qStartIdx + 1, (url.length - qStartIdx));
        if (queryStr.length > 0) {
            var pairs = queryStr.split('&');
            var len   = pairs.length;
            var pair  = [];
            for (var i = 0; i < len; i++) {
                // Is it a valid key value pair?
                if (pairs[i].search('=') !== -1) {
                    pair            = pairs[i].split('=');
                    result[pair[0]] = pair[1];
                }
            }

            return result;
        } else {
            return result;
        }
    }//end if

};

/**
 * Adds given (var => value) list to the given URLs query string.
 */
dfx.addToQueryString = function(url, addQueries)
{
    var mergedUrl        = '';
    var baseUrl          = dfx.baseUrl(url);
    var queryStringArray = dfx.queryString(url);
    mergedQry = dfx.objectMerge(queryStringArray, addQueries);

    var queryStr = '?';
    dfx.foreach(mergedQry, function(key) {
            queryStr = queryStr + key + '=' + mergedQry[key] + '&';
        });

    // More than just a ? to add to the URL?
    if (queryStr.length > 1) {
        // Put the URL together with qry str and take off the trailing &.
        mergedUrl = baseUrl + queryStr.substr(0, (queryStr.length - 1));
    } else {
        mergedUrl = url;
    }

    return mergedUrl;

};


/**
 * Return the filename without the path.
 */
dfx.getFileInputName = function(fileFieldValue)
{
    var filename = '';
    if (fileFieldValue.indexOf('\\') > -1) {
      filename = fileFieldValue.substring(fileFieldValue.lastIndexOf('\\') + 1, fileFieldValue.length);
    }

    if (fileFieldValue.indexOf('/') > -1) {
      filename = fileFieldValue.substring(fileFieldValue.lastIndexOf('/') + 1, fileFieldValue.length);
    }

    if (filename === '') {
        return fileFieldValue;
    }

    return filename;

};


/**
 * Return a unique id.
 */
dfx.getUniqueId = function()
{
    var timestamp = (new Date()).getTime();
    var random    = Math.ceil(Math.random() * 1000000);
    var id        = timestamp + '' + random;
    return id.substr(5, 18).replace(/,/, '');

};

/**
 * Return TRUE if the value exists in an array.
 */
dfx.inArray = function(needle, haystack)
{
    var hln = haystack.length;
    for (var i = 0; i < hln; i++) {
        if (needle === haystack[i]) {
            return true;
        }
    }

    return false;

};

/**
 * Computes the difference of two arrays.
 * If firstOnly is set to TRUE then only the elements that are in first array
 * but not in the second array will be returned.
 */
dfx.arrayDiff = function(array1, array2, firstOnly)
{
    var al  = array1.length;
    var res = [];
    for (var i = 0; i < al; i++) {
        if (dfx.inArray(array1[i], array2) === false) {
            res.push(array1[i]);
        }
    }

    if (firstOnly !== true) {
        al = array2.length;
        for (var i = 0; i < al; i++) {
            if (dfx.inArray(array2[i], array1) === false) {
                res.push(array2[i]);
            }
        }
    }

    return res;

};

/**
 * Merges two objects together
 */
dfx.objectMerge = function (ob1, ob2)
{
    dfx.foreach(ob2, function(key) {
        ob1[key] = ob2[key];
        return true;
    });

    return ob1;

};

dfx.arrayMerge = function (array1, array2)
{
    var c = array2.length;
    for (var i = 0; i < c; i++) {
        array1.push(array2[i]);
    }

    return array1;

};

dfx.removeArrayIndex = function(array, index)
{
    if (!array || dfx.isset(array[index]) === false) {
        return null;
    }

    return array.splice(index, 1);

};

/**
 * Converts the multiple spaces, tabs, new lines to hard-spaces (&nbsp;)
 * NOT COMPLETE
 */
dfx.convertSpaces = function(elem, options)
{
    options = options || {};
    if (dfx.isset(options.newLines) === false) {
        options.newLines = true;
    }

    if (dfx.isset(options.tabs) === false) {
        options.tabs = true;
    }

    // Traverse the elem and replace the spaces.
    var count   = elem.childNodes.length;
    var c       = String.fromCharCode(160);
    var content = null;
    for (var i = 0; i < count; i++) {
        var child = elem.childNodes[i];
        content   = null;
        if (child.nodeType === dfx.TEXT_NODE) {
            content = child.data;
        }

        if (content !== null) {
            // Change \r\n to \n.
            var rep = '';
            content = content.replace(/\r/g, rep);

            // Convert all new lines to <br /> tags.
            if (options.newLines === false) {
                rep = '';
            } else {
                rep = '<br />';
            }

            content = content.replace(/\n/g, rep);

            // Convert tabs to 4 spaces.
            if (options.tabs === false) {
                rep = '';
            } else {
                rep = c + c + c + c;
            }

            content = content.replace(/\t/g, rep);

            if (child.data) {
                // Update child content.
                child.data = content;
            }
        }//end if

        if (child.childNodes && child.childNodes.length > 0) {
            dfx.convertSpaces(child, options);
        }
    }//end for

};

dfx.stripTags = function(content, allowedTags)
{
    var match;
    var re      = new RegExp(/<\/?(\w+)((\s+\w+(\s*=\s*(?:".*?"|'.*?'|[^'">\s]+))?)+\s*|\s*)\/?>/gim);
    var resCont = content;
    while ((match = re.exec(content)) != null) {
        if (dfx.inArray(match[1], allowedTags) !== true) {
            resCont = resCont.replace(match[0], '');
        }
    }

    return resCont;

};

dfx.getImage = function(url, callback)
{
    var img    = new Image();
    img.onload = function() {
        callback.call(this, img);
    };

    img.onerror = function() {
        callback.call(this, false);
    };

    img.src = url;

};


dfx.resizeImage = function(img, size, sizesOnly)
{
    var h = dfx.attr(img, 'height');
    var w = dfx.attr(img, 'width');

    var max = null;
    if ((size instanceof Object) === true) {
        // Rectangle, i.e. max h and max w constraint can be different.
        max = dfx.clone(size);
    } else {
        // Square.
        max = {
            height: size,
            width: size
        };
    }

    if (h === w) {
        // Square, use the smaller one for both.
        var min = Math.min(max.width, max.height);
        h       = min;
        w       = min;
    } else {
        if (w >= max.width || h >= max.height) {
            // Shrink.
            if (w >= max.width) {
                h = (h * (max.width / w));
                w = max.width;
            }

            if (h >= max.height) {
                // Height is still over max, resize again.
                w = (w * (max.height / h));
                h = max.height;
            }
        } else {
            // Enlarge.
            if (w > h) {
                h = (h * (max.width / w));
                w = max.width;
            } else if (h > w) {
                w = (w * (max.height / h));
                h = max.height;
            }
        }//end if
    }//end if

    h = Math.round(h);
    w = Math.round(w);

    if (sizesOnly === true) {
        var result = {
            height: h,
            width: w
        };
        return result;
    } else {
        dfx.attr(img, 'height', h);
        dfx.attr(img, 'width', w);
        return img;
    }

};

dfx.strRepeat = function(str, multiplier)
{
    var rstr = '';
    for (var i = 0; i < multiplier; i++) {
        rstr += str;
    }

    return rstr;

};

dfx.browser = function()
{
    var result     = {};
    result.version = jQuery.browser.version;
    if (jQuery.browser.mozilla === true) {
        result.type = 'mozilla';
    } else if (jQuery.browser.msie === true) {
        result.type = 'msie';
    } else if (jQuery.browser.opera === true) {
        result.type = 'opera';
    } else if (jQuery.browser.safari === true) {
        result.type = 'safari';
    }

    return result;

};

dfx.getElemPositionStyles = function(elem, orientation)
{
    var h       = dfx.getElementHeight(elem);
    var w       = dfx.getElementWidth(elem);
    var res     = {};
    orientation = orientation || Widget.CENTER;

    switch (orientation) {
        case Widget.CENTER:
            res = {
                'margin-top': ((h / 2) * (-1)) + 'px',
                'top': '50%',
                'margin-left': ((w / 2) * (-1)) + 'px',
                'left': '50%'
            };
        break;

        case Widget.TOP:
            res = {
                'margin-top': (h * (-1)) + 'px',
                'top': '0px',
                'margin-left': ((w / 2) * (-1)) + 'px',
                'left': '50%'
            };
        break;

        case Widget.BOTTOM:
            res = {
                'margin-top': (h * (-1)) + 'px',
                'top': '100%',
                'margin-left': ((w / 2) * (-1)) + 'px',
                'left': '50%'
            };
        break;

        case Widget.LEFT:
            res = {
                'margin-top': ((h / 2) * (-1)) + 'px',
                'top': '50%',
                'left': '0px'
            };
        break;

        case Widget.RIGHT:
            res = {
                'margin-top': ((h / 2) * (-1)) + 'px',
                'top': '50%',
                'margin-left': (w * (-1)) + 'px',
                'left': '100%'
            };
        break;

        default:
            // Do nothing.
        break;
    }//end switch

    return res;

};

dfx.htmlspecialchars = function(str)
{
    str = str.replace(/&/g, '&amp;');
    str = str.replace(/"/g, '&quot;');
    str = str.replace(/'/g, '&#039;');
    str = str.replace(/</g, '&lt;');
    str = str.replace(/>/g, '&gt;');
    return str;

};

dfx.readableSize = function(size, unit)
{
    var units = ['B',
                 'kB',
                 'MB',
                 'GB'];

    var maxUnit = (units.length - 1);

    // Accept units as a parameter, maybe...
    if (unit) {
        var index = units.find(unit);
        if (index < 0) {
            unit = null;
        }
    }

    if (unit < 0) {
        unit = 2;
    }

    var factor = 0;
    while (unit !== factor && size >= 1000 && factor < maxUnit) {
        size = (size / 1000);
        factor++;
    }

    var readable = size.toFixed(2) + units[factor];
    return readable;

};

dfx.displayThumbViewer = function(thumb, evt, targetWidget)
{
    var intervalid  = null;
    var thumbViewer = dfx.getId('SplashScreenThumbViewer');
    if (thumbViewer === null) {
        thumbViewer = dfx.createThumbViewer(thumb, evt);
        dfx.hideElement(thumbViewer);
        document.body.appendChild(thumbViewer);
    }

    var oriImage = dfx.getMouseEventTarget(evt);
    dfx.getId('SplashScreenThumbViewer-img').setAttribute('src', oriImage.getAttribute('src'));
    dfx.setThumbViewerText(thumb, evt, function() {
        /*
            TODO: Once IE 8 releases stable version, thumbviewer without
            this code. For some reason, I have to recreate event mask div
            everytime to make it work with IE at the moment.
            21/Nov/2008.
        */
        /*dfx.remove(dfx.getId('SplashScreenThumbViewer-eventMask'));
        var eventMask       = document.createElement('div');
        eventMask.id        = 'SplashScreenThumbViewer-eventMask';
        eventMask.className = 'SplashScreenThumbViewerEventMask';
        eventMask.innerHTML = ' &nbsp;';

        dfx.getId('SplashScreenThumbViewer').appendChild(eventMask);*/

        var thumbWidth   = 54;
        var thumbHeight  = 79;
        var viewerWidth  = 236;
        var viewerHeight = 193;

        var target = dfx.getMouseEventTarget(evt);
        var coords = dfx.getElementCoords(target);
        var left   = (coords.x - (viewerWidth - thumbHeight));
        var top    = (coords.y - (viewerHeight - thumbWidth));

        var scrollY = dfx.getScrollCoords().y;
        if (scrollY > 0) {
            top -= scrollY;
        }

        dfx.setStyle(thumbViewer, 'left', left);
        dfx.setStyle(thumbViewer, 'top', top);

        dfx.showElement(thumbViewer);
        targetWidget.thumbDisplayed = true;

        var setMousePos = function(e) {
            var scrY  = dfx.getScrollCoords().y;
            var pageX = e.pageX;
            var pageY = (e.pageY - scrY);

            if ((pageX < left || pageX > (left + viewerWidth)) || (pageY < top || pageY > (top + viewerHeight))) {
                dfx.hideElement(dfx.getId('SplashScreenThumbViewer'));
                clearInterval(intervalid);
                dfx.stopMousePositionTrack(setMousePos);
            }
        };

        dfx.startMousePositionTrack(setMousePos);
    });

};


dfx.createThumbViewer = function(thumb, evt)
{
    var thumbWrapper       = document.createElement('div');
    thumbWrapper.id        = 'SplashScreenThumbViewer';
    thumbWrapper.className = 'SplashScreenThumbViewerWrapper';

    var imageHolder       = document.createElement('div');
    imageHolder.className = 'SplashScreenThumbViewerImageHolder';

    var description       = document.createElement('div');
    description.id        = 'SplashScreenThumbViewer-desc';
    description.className = 'SplashScreenThumbViewerDescription';

    var oriImage = dfx.getMouseEventTarget(evt);
    var image    = document.createElement('img');
    image.id     = 'SplashScreenThumbViewer-img';
    image.src    = oriImage.getAttribute('src');

    imageHolder.appendChild(image);
    thumbWrapper.appendChild(imageHolder);
    thumbWrapper.appendChild(description);
    return thumbWrapper;

};

dfx.setThumbViewerText = function(thumb, evt, callback)
{
    var oriImage = dfx.getMouseEventTarget(evt);
    var assetid  = oriImage.getAttribute('assetid');
    var version  = oriImage.getAttribute('version');

    if (version === '0') {
        AssetManager.getAsset(assetid, function(asset) {
            dfx.getId('SplashScreenThumbViewer-desc').innerHTML = asset.name;
            callback();
        }, true, {attributes: ['name'],
            type: false,
            typeIcon: false,
            linking: [],
            urls: false}
        );
    } else {
        dfx.getId('SplashScreenThumbViewer-desc').innerHTML = 'Version ' + version;
        callback();
    }

};

dfx.preloadStylesheetImages = function(prefix, defaultBaseUrl)
{
    prefix         = prefix || [];
    defaultBaseUrl = defaultBaseUrl || '/';

    var styleSheets = document.styleSheets;
    var sln         = styleSheets.length;

    for (var i = 0; i < sln; i++) {
        var baseUrl  = '';
        var contents = '';
        if (styleSheets[i].href) {
            // Get the base URL.
            baseUrl = styleSheets[i].href.substring(0, styleSheets[i].href.lastIndexOf('/'));
        }

        if (baseUrl !== '') {
            baseUrl += '/';
        } else {
            baseUrl = defaultBaseUrl;
        }

        if (styleSheets[i].cssRules) {
             var sheetRules = styleSheets[i].cssRules;
             var rln        = sheetRules.length;
             for (var j = 0; j < rln; j++) {
                 contents += sheetRules[j].cssText;
             }
        } else {
             contents += styleSheets[i].cssText;
        }

        var re      = '(' + prefix.join('|') + ')[^\(]+\.(gif|jpg|png)';
        var regExp  = new RegExp(re, 'g');
        var imgUrls = contents.match(regExp);
        if (imgUrls !== null && imgUrls.length > 0) {
            dfx.foreach(imgUrls, function(key) {
                var img = new Image();
                img.src = baseUrl + imgUrls[key];
            });
        }
    }//end for

};

dfx.getFileExtension = function(filename)
{
    var parts = filename.split('.');
    if (parts.length === 1) {
        return '';
    }

    var ext = parts[(parts.length - 1)].toLowerCase();
    return ext;

};

dfx.entitiesArray = {
    160 : '&nbsp;',
    161 : '&iexcl;',
    162 : '&cent;',
    163 : '&pound;',
    164 : '&curren;',
    165 : '&yen;',
    166 : '&brvbar;',
    167 : '&sect;',
    168 : '&uml;',
    169 : '&copy;',
    170 : '&ordf;',
    171 : '&laquo;',
    172 : '&not;',
    173 : '&shy;',
    174 : '&reg;',
    175 : '&macr;',
    176 : '&deg;',
    177 : '&plusmn;',
    178 : '&sup2;',
    179 : '&sup3;',
    180 : '&acute;',
    181 : '&micro;',
    182 : '&para;',
    183 : '&middot;',
    184 : '&cedil;',
    185 : '&sup1;',
    186 : '&ordm;',
    187 : '&raquo;',
    188 : '&frac14;',
    189 : '&frac12;',
    190 : '&frac34;',
    191 : '&iquest;',
    192 : '&Agrave;',
    193 : '&Aacute;',
    194 : '&Acirc;',
    195 : '&Atilde;',
    196 : '&Auml;',
    197 : '&Aring;',
    198 : '&AElig;',
    199 : '&Ccedil;',
    200 : '&Egrave;',
    201 : '&Eacute;',
    202 : '&Ecirc;',
    203 : '&Euml;',
    204 : '&Igrave;',
    205 : '&Iacute;',
    206 : '&Icirc;',
    207 : '&Iuml;',
    208 : '&ETH;',
    209 : '&Ntilde;',
    210 : '&Ograve;',
    211 : '&Oacute;',
    212 : '&Ocirc;',
    213 : '&Otilde;',
    214 : '&Ouml;',
    215 : '&times;',
    216 : '&Oslash;',
    217 : '&Ugrave;',
    218 : '&Uacute;',
    219 : '&Ucirc;',
    220 : '&Uuml;',
    221 : '&Yacute;',
    222 : '&THORN;',
    223 : '&szlig;',
    224 : '&agrave;',
    225 : '&aacute;',
    226 : '&acirc;',
    227 : '&atilde;',
    228 : '&auml;',
    229 : '&aring;',
    230 : '&aelig;',
    231 : '&ccedil;',
    232 : '&egrave;',
    233 : '&eacute;',
    234 : '&ecirc;',
    235 : '&euml;',
    236 : '&igrave;',
    237 : '&iacute;',
    238 : '&icirc;',
    239 : '&iuml;',
    240 : '&eth;',
    241 : '&ntilde;',
    242 : '&ograve;',
    243 : '&oacute;',
    244 : '&ocirc;',
    245 : '&otilde;',
    246 : '&ouml;',
    247 : '&divide;',
    248 : '&oslash;',
    249 : '&ugrave;',
    250 : '&uacute;',
    251 : '&ucirc;',
    252 : '&uuml;',
    253 : '&yacute;',
    254 : '&thorn;',
    255 : '&yuml;',
    402 : '&fnof;',
    913 : '&Alpha;',
    914 : '&Beta;',
    915 : '&Gamma;',
    916 : '&Delta;',
    917 : '&Epsilon;',
    918 : '&Zeta;',
    919 : '&Eta;',
    920 : '&Theta;',
    921 : '&Iota;',
    922 : '&Kappa;',
    923 : '&Lambda;',
    924 : '&Mu;',
    925 : '&Nu;',
    926 : '&Xi;',
    927 : '&Omicron;',
    928 : '&Pi;',
    929 : '&Rho;',
    931 : '&Sigma;',
    932 : '&Tau;',
    933 : '&Upsilon;',
    934 : '&Phi;',
    935 : '&Chi;',
    936 : '&Psi;',
    937 : '&Omega;',
    945 : '&alpha;',
    946 : '&beta;',
    947 : '&gamma;',
    948 : '&delta;',
    949 : '&epsilon;',
    950 : '&zeta;',
    951 : '&eta;',
    952 : '&theta;',
    953 : '&iota;',
    954 : '&kappa;',
    955 : '&lambda;',
    956 : '&mu;',
    957 : '&nu;',
    958 : '&xi;',
    959 : '&omicron;',
    960 : '&pi;',
    961 : '&rho;',
    962 : '&sigmaf;',
    963 : '&sigma;',
    964 : '&tau;',
    965 : '&upsilon;',
    966 : '&phi;',
    967 : '&chi;',
    968 : '&psi;',
    969 : '&omega;',
    977 : '&thetasym;',
    978 : '&upsih;',
    982 : '&piv;',
    8226 : '&bull;',
    8230 : '&hellip;',
    8242 : '&prime;',
    8243 : '&Prime;',
    8254 : '&oline;',
    8260 : '&frasl;',
    8472 : '&weierp;',
    8465 : '&image;',
    8476 : '&real;',
    8482 : '&trade;',
    8501 : '&alefsym;',
    8592 : '&larr;',
    8593 : '&uarr;',
    8594 : '&rarr;',
    8595 : '&darr;',
    8596 : '&harr;',
    8629 : '&crarr;',
    8656 : '&lArr;',
    8657 : '&uArr;',
    8658 : '&rArr;',
    8659 : '&dArr;',
    8660 : '&hArr;',
    8704 : '&forall;',
    8706 : '&part;',
    8707 : '&exist;',
    8709 : '&empty;',
    8711 : '&nabla;',
    8712 : '&isin;',
    8713 : '&notin;',
    8715 : '&ni;',
    8719 : '&prod;',
    8721 : '&sum;',
    8722 : '&minus;',
    8727 : '&lowast;',
    8730 : '&radic;',
    8733 : '&prop;',
    8734 : '&infin;',
    8736 : '&ang;',
    8743 : '&and;',
    8744 : '&or;',
    8745 : '&cap;',
    8746 : '&cup;',
    8747 : '&int;',
    8756 : '&there4;',
    8764 : '&sim;',
    8773 : '&cong;',
    8776 : '&asymp;',
    8800 : '&ne;',
    8801 : '&equiv;',
    8804 : '&le;',
    8805 : '&ge;',
    8834 : '&sub;',
    8835 : '&sup;',
    8836 : '&nsub;',
    8838 : '&sube;',
    8839 : '&supe;',
    8853 : '&oplus;',
    8855 : '&otimes;',
    8869 : '&perp;',
    8901 : '&sdot;',
    8968 : '&lceil;',
    8969 : '&rceil;',
    8970 : '&lfloor;',
    8971 : '&rfloor;',
    9001 : '&lang;',
    9002 : '&rang;',
    9674 : '&loz;',
    9824 : '&spades;',
    9827 : '&clubs;',
    9829 : '&hearts;',
    9830 : '&diams;'
};

dfx.fixHtml = function(html)
{
    var newHtml = '';
    var ln      = html.length;
    for (i = 0; i < ln; i++) {
        code = html.charCodeAt(i);
        if (code > 127) {
            entity = dfx.entitiesArray[code];
            if (entity) {
                newHtml += entity;
            } else {
                newHtml += '&#' + code + ';';
            }
        } else {
            newHtml += html.charAt(i);
        }
    }

    return newHtml;

};

/* @codingStandardsIgnoreStart */
if (!window.console) {
    window.console = {};
    window.console.log = function() {};
    window.console.info = function() {};
}
/* @codingStandardsIgnoreEnd */

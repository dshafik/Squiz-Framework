if (!window.dfx) {
    window.dfx = function() {};
}

/**
 * Sets a CSS style property on an element.
 *
 * The property name can be passed in as either javascript style notation
 * (fontHeight) or CSS notation (font-height).
 *
 * @param {DomElement} element  The element to apply the style to.
 * @param {String}     property The name of the property to set
 *                              eg. backgroundColor, borderWidth etc.
 * @param {String}     value    The value to set the property to.
 *
 * @type void
 */
dfx.setStyle = function(element, property, value)
{
    if (element) {
        jQuery(element).css(property, value);
    }

};


/**
 * Sets a CSS style property on an array of elements.
 *
 * The property name can be passed in as either javascript style notation
 * (fontHeight) or CSS notation (font-height).
 *
 * @param {Array(DomElement)} elements The elements to apply the style to.
 * @param {String}            property The name of the property to set
 *                                     eg. backgroundColor, borderWidth etc.
 * @param {String}            value    The value to set the property to.
 *
 * @type void
 */
dfx.setStyles = function(elements, property, value)
{
    jQuery(elements).css(property, value);

};


/**
 * Gets a CSS style property of an element.
 *
 * @param {DomElement} element  The element to retrieve the style from.
 * @param {String}     property The name of the property to get in CSS notation
 *                              eg. background-color, border-width etc.
 *
 * @return The value of the passed style for the passed element.
 * @type   string
 */
dfx.getStyle = function(element, property)
{
    return jQuery(element).css(property);

};


/**
 * Determines if the element has the passed class applied to it.
 *
 * @param {DomElement} element   The element to check.
 * @param {String}     className The class to check for.
 *
 * @return TRUE if the element has the class applied.
 * @type   bool
 */
dfx.hasClass = function(element, className)
{
    return jQuery(element).hasClass(className);

};


/**
 * Applies the passed class to the element.
 *
 * @param {DomElement} element   The element to apply the class to.
 * @param {String}     classNames The classes to apply (separated by spaces).
 *
 * @return void
 * @type   void
 */
dfx.addClass = function(element, classNames)
{
    jQuery(element).addClass(classNames);

};


/**
 * Removes the passed class from the element.
 *
 * @param {DomElement} element   The element to remove the class from.
 * @param {String}     classNames The classes to remove (separated by spaces).
 *
 * @return void
 * @type   void
 */
dfx.removeClass = function(element, classNames)
{
    jQuery(element).removeClass(classNames);

};


/**
 * Removes the old class from the element, and add a new one.
 *
 * Just a shortcut function for add/remove class.
 *
 * @param {DomElement} element       The element to remove the class from.
 * @param {String}     oldClassNames The classes to remove (separated by spaces).
 * @param {String}     newClassNames The classes to add (separated by spaces).
 *
 * @return void
 * @type   void
 */
dfx.swapClass = function(element, oldClassName, newClassName)
{
    if (dfx.hasClass(element, oldClassName) === true) {
        dfx.removeClass(element, oldClassName);
    }

    dfx.addClass(element, newClassName);

};


/**
 * Converts a property like background-color to backgroundColor.
 *
 * @param {String} property The string to convert.
 *
 * @return The converted string.
 * @type   string
 */
dfx.camelCase = function(property)
{
    // Regular expression to find the next hyphen followed by a letter and to
    // seperate the letter in the results.
    var hyphenTest = /-([a-z])/;
    // While there is a hyphen in the string (reg.test == true) replace it with
    // its' trailing letter uppercased.
    while (hyphenTest.test(property) == true) {
        property = property.replace(hyphenTest, RegExp.$1.toUpperCase());
    }

    return property;

};


/**
 * Returns a HEX colour code from a retrieved value.
 *
 * If the passed value is an rgb value, it will be parsed out and returned.
 * If the passed value is already hex, it will be returned intact.
 * If the passed value is a word color code, then ?....
 *
 * @param {String} colour.
 *
 * @return A HEX colour code.
 * @type   string
 */
dfx.getHexColourCode = function(colour)
{
    if (colour.substring(0, 1) === '#') {
        // If the value is a shorthand version (eg #000 instead of #000000) then
        // we want to fill in the specific values.
        if (colour.length === 4) {
            colour = colour.replace(/#(\d|[a-fA-F])(\d|[a-fA-F])(\d|[a-fA-F])/, '#$1$1$2$2$3$3');
        }

        return colour;
    }

    if (colour.substring(0, 3) === 'rgb') {
        // Extract the values from the rgb colour.
        var rgb = colour.match(/rgb\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)/);
        var r   = parseInt(rgb[1]).toString(16);
        var g   = parseInt(rgb[2]).toString(16);
        var b   = parseInt(rgb[3]).toString(16);

        var hexColour = '#' + r + g + b;
        return hexColour;
    }

    // Must be a colour name.
    return dfx.getColourNameValue(colour);

};

/**
 * Returns an RGB colour code from a retrieved value.
 *
 * If the passed value is an rgb value, it will be parsed out and returned.
 * If the passed value is already hex, it will be returned intact.
 * If the passed value is a word color code, then ?....
 *
 * @param {String} colour.
 *
 * @return An RGB colour code.
 * @type   string
 */
dfx.getRGBColourCode = function(colour)
{
    if (colour.substring(0, 1) === '#') {
        // Need to get its' components and format it correctly.
        var components   = dfx.getColourComponents(colour);
        var colourString = 'rgb(' + components.r + ',' + components.g + ',' + components.b + ')';
        return colourString;
    }

    if (colour.substring(0, 3) === 'rgb') {
        // It is already correctly formatted, return it.
        return colour;
    }

    // Must be a colour name.
    return dfx.getRGBColourCode(dfx.getColourNameValue(colour));

};


/**
 * Retrieves R, G, B components from a colour string.
 *
 * @param {String} colour The colour to retrieve the components from.
 *
 * @return An array with R, G and B indexes.
 * @type   Array
 */
dfx.getColourComponents = function(colour)
{
    var components = {};
    if (colour.substring(0, 1) === '#') {
        // This may be a 3 digit code, so we want to expand it first.
        colour = dfx.getHexColourCode(colour);
        // Need to get the components out of the HEX string.
        var rgb      = colour.match(/#([a-fA-F\d]{2})([a-fA-F\d]{2})([a-fA-F\d]{2})/);
        components.r = parseInt(rgb[1], 16);
        components.g = parseInt(rgb[2], 16);
        components.b = parseInt(rgb[3], 16);
    } else if (colour.substring(0, 3) === 'rgb') {
        // It is correctly formatted, just need to extract the digits.
        var rgb      = colour.match(/rgb\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)/);
        components.r = rgb[1];
        components.g = rgb[2];
        components.b = rgb[3];
    } else {
        // Word colour so we call this method again with the hex value of the
        // colour.
        components = dfx.getColourComponents(dfx.getColourNameValue(colour));
    }

    return components;

};


/**
 * Toggle visibility of an element.
 *
 * @param {DomElement} element The element to toggle.
 *
 * @return void
 * @type   void
 */
dfx.toggle = function(element)
{
    jQuery(element).toggle();

};


/**
 * Sets the opacity value of element with respect to differences in browsers.
 *
 * This method requires testing for the existence of MSIE as there is no way to
 * determine the opacity of an element without using the filter property in
 * Internet Explorer.
 *
 * @param {DomElement} element The element to set opacity for.
 * @param {Float}      value   The decimal value to set the opacity to.
 *                             1.0 for full visibility, 0 for transparent.
 *
 * @return void
 * @type   void
 */
dfx.setOpacity = function(element, value)
{
    dfx.setStyle(element, 'opacity', value);

};


/**
 * Retrieve the current opacity of the element.
 *
 * This method requires testing for the existence of MSIE as there is no way to
 * determine the opacity of an element without using the filter property in
 * Internet Explorer.
 *
 * @param {DomElement}
 *
 * @return The decimal value of the element's current opacity.
 * @type   Float
 */
dfx.getOpacity = function(element)
{
    return dfx.getStyle(element, 'opacity');

};

/*
 * String of colour names to colour values.
 *
 * @type String
 */
dfx.colourNameString = '/aliceblue#f0f8ff/antiquewhite#faebd7/aqua#00ffff/aquamarine#7fffd4/azure#f0ffff/beige#f5f5dc/bisque#ffe4c4/black#000000/blanchedalmond#ffebcd/blue#0000ff/blueviolet#8a2be2/brown#a52a2a/burlywood#deb887/cadetblue#5f9ea0/chartreuse#7fff00/chocolate#d2691e/coral#ff7f50/cornflowerblue#6495ed/cornsilk#fff8dc/crimson#dc143c/cyan#00ffff/darkblue#00008b/darkcyan#008b8b/darkgoldenrod#b8860b/darkgray#a9a9a9/darkgrey#a9a9a9/darkgreen#006400/darkkhaki#bdb76b/darkmagenta#8b008b/darkolivegreen#556b2f/darkorange#ff8c00/darkorchid#9932cc/darkred#8b0000/darksalmon#e9967a/darkseagreen#8fbc8f/darkslateblue#483d8b/darkslategray#2f4f4f/darkslategrey#2f4f4f/darkturquoise#00ced1/darkviolet#9400d3/deeppink#ff1493/deepskyblue#00bfff/dimgray#696969/dimgrey#696969/dodgerblue#1e90ff/firebrick#b22222/floralwhite#fffaf0/forestgreen#228b22/fuchsia#ff00ff/gainsboro#dcdcdc/ghostwhite#f8f8ff/gold#ffd700/goldenrod#daa520/gray#808080/grey#808080/green#008000/greenyellow#adff2f/honeydew#f0fff0/hotpink#ff69b4/indianred #cd5c5c/indigo #4b0082/ivory#fffff0/khaki#f0e68c/lavender#e6e6fa/lavenderblush#fff0f5/lawngreen#7cfc00/lemonchiffon#fffacd/lightblue#add8e6/lightcoral#f08080/lightcyan#e0ffff/lightgoldenrodyellow#fafad2/lightgray#d3d3d3/lightgrey#d3d3d3/lightgreen#90ee90/lightpink#ffb6c1/lightsalmon#ffa07a/lightseagreen#20b2aa/lightskyblue#87cefa/lightslategray#778899/lightslategrey#778899/lightsteelblue#b0c4de/lightyellow#ffffe0/lime#00ff00/limegreen#32cd32/linen#faf0e6/magenta#ff00ff/maroon#800000/mediumaquamarine#66cdaa/mediumblue#0000cd/mediumorchid#ba55d3/mediumpurple#9370d8/mediumseagreen#3cb371/mediumslateblue#7b68ee/mediumspringgreen#00fa9a/mediumturquoise#48d1cc/mediumvioletred#c71585/midnightblue#191970/mintcream#f5fffa/mistyrose#ffe4e1/moccasin#ffe4b5/navajowhite#ffdead/navy#000080/oldlace#fdf5e6/olive#808000/olivedrab#6b8e23/orange#ffa500/orangered#ff4500/orchid#da70d6/palegoldenrod#eee8aa/palegreen#98fb98/paleturquoise#afeeee/palevioletred#d87093/papayawhip#ffefd5/peachpuff#ffdab9/peru#cd853f/pink#ffc0cb/plum#dda0dd/powderblue#b0e0e6/purple#800080/red#ff0000/rosybrown#bc8f8f/royalblue#4169e1/saddlebrown#8b4513/salmon#fa8072/sandybrown#f4a460/seagreen#2e8b57/seashell#fff5ee/sienna#a0522d/silver#c0c0c0/skyblue#87ceeb/slateblue#6a5acd/slategray#708090/slategrey#708090/snow#fffafa/springgreen#00ff7f/steelblue#4682b4/tan#d2b48c/teal#008080/thistle#d8bfd8/tomato#ff6347/turquoise#40e0d0/violet#ee82ee/wheat#f5deb3/white#ffffff/whitesmoke#f5f5f5/yellow#ffff00/yellowgreen#9acd32/';


/**
 * Method to convert a css colour name like blue or red to a HEX colour code.
 *
 * @param {String} colourName The name of the colour to convert.
 *
 * @return A string containing the HEX colour code of the colour string.
 * @type   String
 */
dfx.getColourNameValue = function(colourName)
{
    colourName  = colourName.toLowerCase();
    var reg     = new RegExp('\/' + colourName + '(#[a-f0-9]{6})\/');
    var matches = reg.exec(dfx.colourNameString);
    if (matches) {
        return matches[1];
    }

    return '#000000';

};

/**
 * Shows the element so it becomes visible to the user.
 *
 * @param {DomElement} element The element to show.
 *
 * @return void
 * @type void
 */
dfx.showElement = function(element, visibilityOnly)
{
    dfx.setStyle(element, 'visibility', 'visible');
    if (dfx.isset(visibilityOnly) === false || visibilityOnly === false) {
        dfx.setStyle(element, 'display', 'block');
    }

};


/**
 * Hides the element so it becomes invisible to the user.
 *
 * @param {DomElement} element The element to hide.
 *
 * @return void
 * @type void
 */
dfx.hideElement = function(element, visibilityOnly)
{
    dfx.setStyle(element, 'visibility', 'hidden');
    if (dfx.isset(visibilityOnly) === false || visibilityOnly === false) {
        dfx.setStyle(element, 'display', 'none');
    }

};


/**
 * Returns TRUE if the specified element is visible to the user.
 *
 * @param {DomElement} element The element to show.
 *
 * @return TRUE if the specified element is visible to the user.
 * @type boolean
 */
dfx.isShowing = function(element)
{
    var display = dfx.getStyle(element, 'display');
    // We need to check to see if the display is null, as in safari, it will
    // return null if the display is set to none. Similarly for visiblity.
    if (display === 'none' || !display) {
        return false;
    }

    var visibility = dfx.getStyle(element, 'visibility');
    if (visibility === 'hidden' || !visibility) {
        return false;
    }

    return true;

};

dfx.toggleClass = function(elems, className)
{
    jQuery(elems).toggleClass(className);

};


/**
 * Returns the computed styles for given element.
 */
dfx.getComputedStyle = function (el, styleName)
{
    if (styleName) {
        styleName = dfx.camelCase(styleName);
    }

    if (document.defaultView && document.defaultView.getComputedStyle) {
        var styles = document.defaultView.getComputedStyle(el, null);
        if (styleName) {
            return styles[styleName];
        }

        return styles;
    } else if (el.currentStyle) {
        if (styleName) {
            return el.currentStyle[styleName];
        }

        return el.currentStyle;
    }

};

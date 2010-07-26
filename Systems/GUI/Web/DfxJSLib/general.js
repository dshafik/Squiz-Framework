if (!window.dfx) {
    window.dfx = function() {};
}

/**
 * General functions that must be included on every execution.
 */


/**
 * Implements inheritance for two classes.
 *
 * @param {funcPtr} child        The class that is inheriting the parent methods.
 * @param {funcPtr} parent       The parent that is being implemented.
 * @param {string}  noWidgetType If TRUE, 'WidgetType' string is not appended.
 *
 * @return void
 * @type   void
 */
dfx.inherits = function(child, parent, noWidgetType)
{
    if (parent === 'Widget') {
        parent = 'AbstractWidget';
    }

    var parentObj = null;
    if (noWidgetType) {
        parentObj = window[parent];
    } else {
        parentObj = window[parent + 'WidgetType'];
    }

    if (parentObj) {
        dfx.noInclusionInherits(child, parent, noWidgetType);
    } else {
        Widget.prototype.includeWidget(parent, function() {
            dfx.noInclusionInherits(child, parent, noWidgetType);
        });
    }

};//end inherits()


/**
 * Implements inheritance for two classes.
 *
 * The main difference with inherits() function is that
 * this does not include parent widget type before inheritance operation.
 *
 * @param {funcPtr} child        The class that is inheriting the parent methods.
 * @param {funcPtr} parent       The parent that is being implemented.
 * @param {string}  noWidgetType If TRUE, 'WidgetType' string is not appended.
 *
 * @return void
 * @type   void
 */
dfx.noInclusionInherits = function(child, parent, noWidgetType)
{
    if (parent instanceof String || typeof parent === 'string') {
        if (noWidgetType) {
            parent = window[parent];
        } else {
            parent = window[parent + 'WidgetType'];
        }
    }

    if (child instanceof String || typeof child === 'string') {
        if (noWidgetType) {
            child = window[child];
        } else {
            child = window[child + 'WidgetType'];
        }
    }

    var above = function(){};
    if (dfx.isset(parent) === true) {
        for (value in parent.prototype) {
            // If the child method already exists, move this method to the parent
            // so it can be called using super.method().
            if (child.prototype[value]) {
                above.prototype[value] = parent.prototype[value];
                continue;
            }

            child.prototype[value] = parent.prototype[value];
        }
    }

    if (child.prototype) {
        above.prototype.constructor = parent;
        child.prototype['super']    = new above();
    }

};//end noInclusionInherits()


/*
 * Addon element for array that allows checking for existence of an element.
 *
 * @param {mixed} value The value to search for in the array.
 *
 * @return TRUE if the array contains the passed value.
 * @type   boolean
 */
Array.prototype.inArray = function(value)
{
    if (typeof(value) === 'string' && this.length > 1) {
       // Attempt at an optimisation for string searching. Flattens the array,
       // and performs a regular expression search on it.
       var flatArray = this.toString();
       var pattern   = new RegExp('(' + value + ',|,' + value + ')', 'g');
       return pattern.test(flatArray);
    }

    // Value is not a string, so the array must be iterated through and each
    // element checked against the search string.
    var len = this.length;
    for (var i = 0; i < len; i++) {
        if (this[i] === value) {
            return true;
        }
    }

    return false;

};//end Array.prototype.inArray()

/*
 * Searches the array for the given item.
 *
 * Returns the item index if found, else false.
 *
 * @param array array The array.
 * @param mixed item  Item to search in the specified array.
 *
 * @return int
 */
Array.prototype.find = function(item)
{
    var length = this.length;
    for (var i = 0; i < length; i++) {
        if (this[i] === item) {
            return i;
        }
    }

    return -1;

};//end Array.prototype.arraySearch()

/*
 * Merge an HTMLCollection into this array.
 *
 * Some objects have the properties of an array, but cannot be 'concat()'ed to
 * normal arrays. This fixes that functionality.
 *
 * @param HTMLCollection collection The collection to be merged. This is usually
 *                                  returned from getElementsByTagname().
 *
 * @return void
 * @type   void
 */
Array.prototype.mergeCollection = function(collection)
{
    if (!collection) {
        return;
    }

    var len = collection.length;
    for (var i = 0; i < len; i++) {
        this.push(collection[i]);
    }

};//end Array.prototype.mergeCollection()

Array.prototype.unique = function()
{
    var a = [];
    var l = this.length;
    for (var i = 0; i < l; i++) {
        if (a.find(this[i]) < 0) {
            a.push(this[i]);
        }
    }

    return a;

};


/**
 * Moves all array elements after specified index up by 1.
 *
 * @param {array} array Array to modify
 * @param {int}   index Index that will be removed.
 *
 * @return {array}
 */
function shiftArrayElements(array, index)
{
    // Shift all elements after index up by 1.
    var len = array.length;
    for (var i = parseInt(index); i < (len - 1); i++) {
        var n    = i + 1;
        array[i] = array[n];
    }

    array.pop();
    return array;

};//end shiftArrayElements()


/**
 * Loop through object or array.
 */
dfx.foreach = function(value, cb)
{
    if (value instanceof Array) {
        var len = value.length;
        for (var i = 0; i < len; i++) {
            var res = cb.call(this, i);
            if (res === false) {
                break;
            }
        }
    } else {
        for (var id in value) {
            if (value.hasOwnProperty(id) === true) {
                var res = cb.call(this, id);
                if (res === false) {
                    break;
                }
            }
        }
    }

};//end foreach()

dfx.isEmpty = function(value)
{
    if (value) {
        if (value instanceof Array) {
            if (value.length > 0) {
                return false;
            }
        } else {
            for (var id in value) {
                if (value.hasOwnProperty(id) === true) {
                    return false;
                }
            }
        }
    }

    return true;

};

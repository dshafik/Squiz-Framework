/**
 * JS code to include all library JS files.
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

// Defines which files will be included. These must be in the same directory as
// the My4.js file.
var my4LibraryString = 'jquery.js|jquery.ui.js|jquery.mousewheel.js|dfx.js|event.js|dom.js|css.js|general.js|effects.js|ajax.js|util.js|json.js|date.js|jquery.scrollTo.js|jquery.localscroll.js';
var my4Scripts       = document.getElementsByTagName('script');
var path             = null;

// Loop through all the script tags that exist in the document and find the one
// that has included this file.
var my4ScriptsLen = my4Scripts.length;
for (var i = 0; i < my4ScriptsLen; i++) {
    if (my4Scripts[i].src) {
        if (my4Scripts[i].src.match(/mysource\.js/)) {
            // We have found our appropriate <script> tag that includes the My4
            // library, so we can extract the path and include the rest.
            path = my4Scripts[i].src.replace(/mysource\.js/,'');
        }
    }
}

// Obtain an array of files that we have specified to include.
var my4LibraryFiles = my4LibraryString.split('|');

// Loop through each files and write out a script tag for each.
var my4LibraryFilesLen = my4LibraryFiles.length;
for (var j = 0; j < my4LibraryFilesLen; j++) {
    var my4IncludeFile = path + my4LibraryFiles[j];
    document.write('<script type="text/javascript" src="' + my4IncludeFile + '"></script>');
}
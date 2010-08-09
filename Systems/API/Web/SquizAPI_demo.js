/**
 * Squiz API demo file.
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

sfapi.rootUrl = 'http://squiz-search.net';

function log(msg) {
    document.getElementById('target').innerHTML = document.getElementById('target').innerHTML + '<br />' + msg;
}

function clearlog() {
    document.getElementById('target').innerHTML = ' ';
}

function demoAPI(systemid, actionid, options) {
    clearlog();
    sfapi.get(systemid, actionid, options, function(data) {
        log(data.result);
    });
}
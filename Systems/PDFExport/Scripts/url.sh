#!/bin/bash
##
# This script opens a new Firefox window and generates a PDF of a URL.
#
# arg1 - where to store the image
# arg2 - the URL to generate
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program as the file license.txt. If not, see
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
#
# @package    Framework
# @subpackage PDFExport
# @author     Squiz Pty Ltd <products@squiz.net>
# @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
# @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
##

DISPLAY=:1 iceweasel -printmode pdf -printfile $2 -print $3

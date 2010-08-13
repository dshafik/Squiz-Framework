#!/bin/bash
##
# This script creates an image of the first page of a PDF.
#
# arg1 - the path to the PDF
# arg2 - where to store the image
# arg3 - the width of the image to create
# arg4 - the height of the image to create
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
# @subpackage ImageExport
# @author     Squiz Pty Ltd <products@squiz.net>
# @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
# @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
##

convert -scale $3x$4 "$1[0]" -background white -gravity center -extent $3x$4 $2

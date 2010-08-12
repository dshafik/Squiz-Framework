<?php
/**
 * DotCreator Library.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Library for creating DOT files.
 */
class DotCreator
{

    /**
     * List of nodes.
     *
     * @var array
     */
    private $_nodes = array();

    /**
     * List of node connections.
     *
     * @var array
     */
    private $_nodeConnections = array();

    /**
     * Path of the DOT template file.
     *
     * @var string
     */
    private $_templateFile = '';

    /**
     * If true then any dupe connections will be shown on 1 line.
     *
     * @var boolean
     */
    private $_singleConnections = TRUE;


    /**
     * Constructor.
     */
    public function __construct()
    {
        $this->_templateFile = dirname(__FILE__).'/template.inc';

    }//end __construct()


    /**
     * Adds a new node, if nodeid already exists then it will be ignored.
     *
     * @param string $nodeid Id of the new node.
     * @param string $label  Label of the node.
     *
     * @return void
     */
    public function addNode($nodeid, $label)
    {
        if (isset($this->_nodes[$nodeid]) === TRUE) {
            $this->_nodes[$nodeid]['count']++;
            $this->_nodes[$nodeid]['label'] = $label.' ['.$this->_nodes[$nodeid]['count'].']';
            // TODO: could track how many times fn is called here?
            return;
        }

        $this->_nodes[$nodeid] = array(
                                  'label' => $label.' [1]',
                                  'count' => 1,
                                 );

    }//end addNode()


    /**
     * Adds a node connection between two specified nodes.
     *
     * @param string $from   The id of the from node.
     * @param string $to     The id of the to node.
     * @param string $label  The label for the connection.
     * @param string $style  The style for the connection.
     * @param string $colour The colour for the connection.
     * @param string $url    Url for the connection.
     *
     * @return void
     */
    public function addConnection($from, $to, $label=NULL, $style=NULL, $colour=NULL, $url=NULL)
    {
        if (empty($from) === TRUE || empty($to) === TRUE) {
            return;
        }

        $con = array(
                'from'   => $from,
                'to'     => $to,
                'label'  => $label,
                'style'  => $style,
                'colour' => $colour,
                'count'  => 1,
                'url'    => $url,
               );

        if ($this->_singleConnections === TRUE) {
            $key = $from.'->'.$to;
            if (isset($this->_nodeConnections[$key]) === TRUE) {
                // Yeah += for label.. for now..
                $this->_nodeConnections[$key]['label'] += $label;
                $this->_nodeConnections[$key]['count']++;
            } else {
                $this->_nodeConnections[$key] = $con;
            }
        } else {
            $this->_nodeConnections[] = $con;
        }

    }//end addConnection()


    /**
     * Returns specified node as a DOT string.
     *
     * @param string $nodeid   Id of the node.
     * @param array  $nodeInfo Node information.
     *
     * @return string
     */
    private function _nodeToStr($nodeid, array $nodeInfo)
    {
        $content = '"'.$nodeid.'" [label="'.$nodeInfo['label']."\"];\n";
        return $content;

    }//end _nodeToStr()


    /**
     * Returns specified node connection as a DOT string.
     *
     * @param array $con Node connection info.
     *
     * @return string
     */
    private function _connectionToStr(array $con)
    {
        $content = '"'.$con['from'].'" -> "'.$con['to'].'"';

        $extra = array();
        if ($con['label'] !== NULL) {
            $label = 'label="'.$con['label'];
            if ($con['count'] > 1) {
                $label .= ' ('.$con['count'].')';

                // Group number of calls.
                $penWidth = 1;
                if ($con['count'] < 5) {
                    $penWidth = 1.5;
                } else if ($con['count'] < 10) {
                    $penWidth = 3;
                } else {
                    $penWidth = 5;
                }

                $extra[] = 'penwidth="'.$penWidth.'"';
            }

            $label  .= '"';
            $extra[] = $label;
        }//end if

        if ($con['style'] !== NULL) {
            $extra[] = 'style="'.$con['style'].'"';
        }

        if ($con['colour'] !== NULL) {
            $extra[] = 'color="'.$con['colour'].'"';
        }

        if (empty($extra) === FALSE) {
            $content .= '['.implode(', ', $extra).']';
        }

        $content .= ";\n";
        return $content;

    }//end _connectionToStr()


    /**
     * Creats the DOT file.
     *
     * @param string $dotFile Path of the result file.
     *
     * @return void
     * @throws Exception When DOT template file cannot be found.
     */
    public function createDotFile($dotFile)
    {
        if (file_exists($this->_templateFile) === FALSE) {
            $msg = 'Cannot find the template file: '.$this->_templateFile;
            throw new Exception($msg);
        }

        $contents = '';

        // Get node list.
        foreach ($this->_nodes as $nodeid => $nodeInfo) {
            $contents .= $this->_nodeToStr($nodeid, $nodeInfo);
        }

        // Connections.
        foreach ($this->_nodeConnections as $con) {
            $contents .= $this->_connectionToStr($con);
        }

        $templateContents = file_get_contents($this->_templateFile);
        $templateContents = str_replace('%graph%', $contents, $templateContents);
        file_put_contents($dotFile, $templateContents);

    }//end createDotFile()


    /**
     * Converts DOT file to SVG format.
     *
     * @param string $dotFile  Path to the DOT file to convert.
     * @param string $destFile Path to the result file.
     *
     * @return string
     * @throws Exception When DOT file cannot be found or fails to convert.
     */
    public function convertToSvg($dotFile, $destFile)
    {
        if (file_exists($dotFile) === FALSE) {
            throw new Exception('Dot file not found: '.$dotFile);
        }

        // Using dot command create the SVG file.
        $cmd  = 'dot -T svg -o '.$destFile;
        $cmd .= ' '.$dotFile;

        $output = '';
        system($cmd, $output);

        // Now wait for SVG creation for at least 5 seconds.
        $seconds = 5;

        $exists = FALSE;
        while ($seconds-- >= 0) {
            // Check if file was created.
            if (file_exists($destFile) === TRUE) {
                $exists = TRUE;
                break;
            }

            sleep(1);
        }

        if ($exists === FALSE) {
            throw new Exception('Failed to create SVG file: '.$output);
        }

        // Insert required JS stuff in to SVG file.
        // First add onload event.
        $svgContents = file_get_contents($destFile);

        $jsOnLoad    = 'onload="parent.SvgDebugger.add(this);"';
        $svgContents = str_replace('<svg', '<svg '.$jsOnLoad, $svgContents);

        file_put_contents($destFile, $svgContents);

        return $destFile;

    }//end convertToSvg()


}//end class

?>

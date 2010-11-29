<?php
/**
 * Actions for the Project System.
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
 * @subpackage Project
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * ProjectActions.
 */
abstract class ProjectActions
{

    /**
     * Request-level cache of the current project ID.
     *
     * This will be set on the first request to getCurrentProjectId(), or can be
     * manually set by setCurrentProjectId().
     *
     * @var integer
     */
    private static $_currentProjectid = NULL;


    /**
     * All systems extending this class must override this method.
     *
     * @return string
     */
    public static function getProjectSystemName()
    {
        return NULL;

    }//end getProjectSystemName()


    /**
     * Returns the current project ID from the current web path.
     *
     * Cached in the $_currentProjectid member variable after the first run.
     *
     * @return integer
     */
    public static function getCurrentProjectId()
    {
        if (self::$_currentProjectid !== NULL) {
            $projectid = self::$_currentProjectid;
        } else {
            $projectid = NULL;

            $url  = Web::stripUrlQueryString(Web::getCurrentUrl());
            $path = parse_url($url, PHP_URL_PATH);

            // Remove leading slash.
            if (substr($path, 0, 1) === '/') {
                $path = substr($path, 1);
            }

            $project = Project::getProjectByPath($path);

            if ($project !== NULL) {
                $projectid = (int) $project['project_id'];
            }
        }//end if

        return $projectid;

    }//end getCurrentProjectId()


    /**
     * Explicitly set the current project ID.
     *
     * @param integer $projectid The project ID to select.
     *
     * @return void
     */
    public static function setCurrentProjectId($projectid)
    {
        if (($projectid === NULL) || (is_int($projectid) === TRUE)) {
            self::$_currentProjectid = $projectid;
        }

    }//end setCurrentProjectId()


    /**
     * Get information about a single project.
     *
     * If the project does not exist, the action returns NULL.
     *
     * @param integer $projectid The ID of the desired project.
     *
     * @return array
     */
    public static function getProject($projectid)
    {
        $project = DAL::executeAssoc(Project::getProjectSystemName(), 'getProject');

        if (count($project) > 0) {
            $project = $project[0];
        } else {
            $project = NULL;
        }

        return $project;

    }//end getProject()


    /**
     * Get the current project's name, if a project is set.
     *
     * @return string
     */
    public static function getCurrentProjectName()
    {
        $projectId = Project::getCurrentProjectId();

        if ($projectId === NULL) {
            $projectName = _('No project active');
        } else {
            $project     = Project::getProject($projectId);
            $projectName = $project['remote_name'];
        }//end if

        return $projectName;

    }//end getCurrentProjectName()


    /**
     * Get information about a single project, identified by its web path.
     *
     * If no project exists at that web path, the action returns NULL.
     *
     * @param integer $webPath The web path of the desired project.
     *
     * @return array
     */
    public static function getProjectByPath($webPath)
    {
        $project = DAL::executeAssoc(Project::getProjectSystemName(), 'getProjectByPath');

        if (count($project) > 0) {
            $project = $project[0];
        } else {
            $project = NULL;
        }

        return $project;

    }//end getProjectByPath()


        /**
     * Get information about all projects.
     *
     * @return array
     */
    public static function getAllProjects()
    {
        $projects = DAL::executeAssoc(Project::getProjectSystemName(), 'getAllProjects');
        return $projects;

    }//end getAllProjects()


    /**
     * Get information about all projects.
     *
     * @return array
     *
     * @api read
     */
    public static function getProjectIds()
    {
        $projectIds = DAL::executeAssoc(Project::getProjectSystemName(), 'getAllProjects', 0);
        return $projectIds;

    }//end getProjectIds()


    /**
     * Returns the number of projects that the current user has the privilege to.
     *
     * @param string $privilege The named privilege.
     *
     * @return array
     */
    public static function getCountProjectsWithPermission($privilege)
    {
        $retval = count(Project::getProjectsWithPermission($privilege));
        return $retval;

    }//end getCountProjectsWithPermission()


    /**
     * Returns a list of projects that the current user has the passed privilege to.
     *
     * @param string $privilege The named privilege.
     *
     * @return array
     */
    public static function getProjectsWithPermission($privilege)
    {
        Channels::includeSystem('Permission');
        Channels::includeSystem('User');
        $retval   = array();
        $user     = User::getCurrentUserid();
        $projects = Project::getAllProjects();

        foreach ($projects as $project) {
            $projectid = (int) $project['project_id'];
            if (Permission::hasPermission($projectid, $privilege) === TRUE) {
                $retval[] = $project;
            }
        }//end foreach

        return $retval;

    }//end getProjectsWithPermission()


    /**
     * Serve a project thumbnail.
     *
     * @param integer $projectId The ID of the project (NULL = current project).
     *
     * @return boolean
     */
    public static function serveProjectThumbnail($projectId=NULL)
    {
        if ($projectId === NULL) {
            $projectId = Project::getCurrentProjectId();
        }

        // Get the default screenshot.
        $destPath = dirname(__FILE__).'/Web/no-screenshot.jpg';
        $mimeType = 'image/jpeg';

        $projectRow = Project::getProject($projectId);
        if ($projectRow !== NULL) {
            $fileName = BaseSystem::getDataDir('Project').'/project-shots/project-'.$projectId.'.jpg';

            if (file_exists($fileName) === TRUE) {
                $destPath = $fileName;
            }
        }

        Channels::includeSystem('GUI');
        GUI::serveFile($destPath, $mimeType);

        return TRUE;

    }//end serveProjectThumbnail()


    /**
     * Returns a list of valid and active projects for the project switcher.
     *
     * @return string
     */
    public static function getProjectSwitcherContent()
    {
        $content  = '';
        $projects = Project::getProjectsWithPermission('dashboard.view');

        foreach ($projects as $project) {
            $projectid = (int) $project['project_id'];
            $content  .= Project::getProjectSwitcherProjectContent($projectid);
        }

        $content = trim($content);
        if (empty($content) === TRUE) {
            // None found, show a message saying that.
            $content .= '<div class="ProjectNoneFound">';
            $content .= _('No projects found.');
            $content .= '</div>';
        }

        return $content;

    }//end getProjectSwitcherContent()


    /**
     * Returns the contents of a project switcher item for an Analytics project.
     *
     * @param string $projectid The ID of the project.
     *
     * @return string
     */
    public static function getProjectSwitcherProjectContent($projectid)
    {
        $project = Project::getProject($projectid);
        $content = '';
        $prefix  = 'ProjectProjectSwitcher-'.$projectid;

        // Arrow styles.
        $upArrow     = '<span class="change-better">&#x25b2;</span>';
        $downArrow   = '<span class="change-worse">&#x25bc;</span>';
        $steadyArrow = '<span class="change-steady">&#x25c4;</span>';

        if ($project !== NULL) {
            $oldProjectid = self::$_currentProjectid;
            Project::setCurrentProjectId($projectid);

            $projectName    = $project['name'];
            $projectBaseUrl = '/'.$project['path'];
            $thumbPath      = $projectBaseUrl.'?tb=1';
            $projectUrl     = $projectBaseUrl;

            $content .= '<a href="'.$projectUrl.'">';
            $content .= '<div id="'.$prefix.'-entry" class="ProjectEntry">';

            // Thumbnail.
            $content .= '<div id="'.$prefix.'-thumbnail" class="ProjectThumbnail">';
            $content .= '<img border="0" src="'.$thumbPath.'" alt="'.$projectName.'" />';
            $content .= '</div>';

            // Details.
            $content .= '<div id="'.$prefix.'-details" class="ProjectDetails">';

            $content .= '<div id="'.$prefix.'-title" class="ProjectDetailsTitle">';
            $content .= '<span id="'.$prefix.'-project-name" class="ProjectDetailsName">';
            $content .= $projectName;
            $content .= '</span>';
            $content .= '<span id="'.$prefix.'-project-link" class="ProjectDetailsUrl">';
            $content .= '</span>';
            $content .= '</div>';

            $content .= '<div id="'.$prefix.'-summary" class="ProjectDetailsSummary">';


            $content .= '</div>';
            $content .= '</div>';
            $content .= '</div>';
            $content .= '</a>';

            Project::setCurrentProjectId($oldProjectid);
        }//end if

        return $content;

    }//end getProjectSwitcherProjectContent()


}
<var name="countProjects">
    <dataProvider action="Project::getCountProjectsWithPermission">
        <arg>dashboard.view</arg>
    </dataProvider>
</var>


<div id="ProjectProjectSelector-background"></div>
<div id="ProjectProjectSelector-inner">

    <div id="ProjectProjectSelector-heading">
        <div id="ProjectProjectSelector-heading-text">
            <text>Choose a project</text>
        </div>
        <condition>
            <if condition="User::isSystemAdministrator()">
                <content>
                    <div id="ProjectProjectSelector-add-button">
                        <widget type="GUI/Button" id="ProjectProjectSelector-button-add-project">
                            <setting name="value"><text>Add New Project</text></setting>
                            <setting name="click">ProjectProjectSelector.addProjectClicked()</setting>
                        </widget>
                    </div>
                </content>
            </if>
        </condition>
    </div>

    <div id="ProjectProjectSelector-content">
        <condition>
            <if condition="{countProjects} > 0">
                <content>
                    <div id="ProjectProjectSelector-content-scroller">
                        <dataProvider action="Project::getProjectSwitcherContent" />
                    </div>
                </content>
            </if>
            <else>
                <content>
                    <div id="ProjectProjectSelector-content-no-projects">
                        <p><text>No projects have been configured.</text></p>
                        <condition>
                            <if condition="User::isSystemAdministrator()">
                                <content>
                                    <p><text>Click &quot;Add New Project&quot; to start this process.</text></p>
                                </content>
                            </if>
                            <else>
                                <content>
                                    <p><text>Please contact your system adminstrators.</text></p>
                                </content>
                            </else>
                        </condition>
                    </div>
                </content>
            </else>
        </condition>
    </div>

</div>

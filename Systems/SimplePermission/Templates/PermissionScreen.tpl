<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<var name="usersFolderid"><dataProvider action="User::getGlobalUsersFolderid" /></var>

<condition>
    <if-not condition="Permission::hasProjects()">
        <content>
            <div id="PermissionScreen">
                <text>There are no projects in the system</text>
            </div>
        </content>
    </if-not>
    <else>
      <content>
        <div id="PermissionScreen">
            <div id="PermissionSettingsBoxContainer" class="GUI-screen-left">
                <!-- Start Permissiont Screen settings box -->
                <widget type="GUI/ScreenSettings" id="projectList">
                    <setting name="title">
                        <text>Projects</text>
                    </setting>
                    <setting name="items">
                        <dataProvider action="Permission::getProjectsListForScreen" />
                    </setting>
                    <setting name="selectable">true</setting>
                    <setting name="removable">false</setting>
                </widget>
                <!-- End Permission Screen settings box -->
            </div>
            <div id="PermissionScreenMainContent" class="GUI-screen-right">
                <dataProvider action="Permission::getPermissionListForScreen" />
            </div>
            <script>
                SimplePermissionPermissionScreen.initScreen({usersFolderid});
            </script>
        </div>
      </content>
    </else>
</condition>

<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<var name="usersFolderid"><dataProvider action="User::getGlobalUsersFolderid" /></var>

<div id="PermissionScreen">
    <div class="PermissionSettingsBoxContainer GUI-screen-left">
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
    <div class="PermissionScreenMainContent GUI-screen-right">
        <dataProvider action="Permission::getPermissionListForScreen" />
    </div>
    <script>
        SimplePermissionPermissionScreen.initScreen({usersFolderid});
    </script>
</div>

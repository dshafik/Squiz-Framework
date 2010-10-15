<div id="RolesScreen">
    <div class="RolesSettingsBoxContainer GUI-screen-left">
        <!-- Start Rolest Screen settings box -->
        <widget type="GUI/ScreenSettings" id="rolesList">
            <setting name="title">
                <text>Roles Setting</text>
            </setting>
            <setting name="subTitle">
                <content>
                    <div style="float:right;">
                        <widget type="GUI/Button" id="addNewRole">
                            <setting name="value"><text>Add New</text></setting>
                            <setting name="click">
                                <function>RoleRoleScreen.createNewRole</function>
                            </setting>
                        </widget>
                    </div>
                </content>
            </setting>
            <setting name="items">
                <dataProvider action="Role::getRoles" />
            </setting>
            <setting name="selectable">true</setting>
            <setting name="removable">true</setting>
        </widget>
        <!-- End Roles Screen settings box -->
    </div>
    <div class="RolesScreenMainContent GUI-screen-right">
        <widget type="GUI/TabPane" id="role-switcher">
            <setting name="tabs">
                <dataProvider action="Role::getPrivilegeListForRolesScreen" />
            </setting>
            <setting name="staticContent">
                <content>
                    <div class="RoleScreen-roleNameSection">
                        <label for="role-name-input"><text>Role Name</text></label>
                        <div style="float:right; padding-top: 2px;">
                            <widget type="GUI/TextBox" id="role-name" />
                        </div>
                    </div>
                </content>
            </setting>
        </widget>
    </div>
    <dataProvider action="Role::getRolesScreenInitCode">
        <arg></arg>
    </dataProvider>
</div>

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
                        <widget type="GUI/Button">
                            <setting name="value"><text>Add New</text></setting>
                            <setting name="click">
                                <function>RoleScreen.createNewRole</function>
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
        <widget type="GUI/Box" id="box-graphs">
            <setting name="headerContent">
                <content>
                    <div style="float:left;">
                        <widget type="GUI/ContentSwitcher" id="role-switcher">
                            <setting name="items">
                                <dataProvider action="Role::getPrivilegeListForRolesScreen" />
                            </setting>
                            <setting name="loadType">static</setting>
                        </widget>
                    </div>
               </content>
           </setting>
           <setting name="content">
                <content>
                    <div class="RoleScreen-roleNameSection">
                        <label for="role-name-input"><text>Role Name</text></label>
                        <div style="float:right;">
                            <widget type="GUI/TextBox" id="role-name" />
                        </div>
                    </div>
                    <dataProvider action="GUIContentSwitcher::getBodyContent">
                        <arg><widget-settings id="role-switcher" /></arg>
                    </dataProvider>
                </content>
            </setting>
       </widget>
    </div>
    <script>RoleScreen.initScreen();</script>
</div>

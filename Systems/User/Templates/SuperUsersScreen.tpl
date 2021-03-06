<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<var name="screenData"><dataProvider action="User::getSuperUsersScreenData" /></var>

<div id="SuperUsersScreen" class="GUI-screen-middle">
    <widget type="GUI/Box" id="superUsers-box">
        <setting name="title"><text>Super Users</text></setting>
        <setting name="headerContent">
            <content>
                <div class="addNewButtonContainer">
                    <widget type="GUI/Button" id="addNewSuperUser">
                        <setting name="value"><text>Add Existing User</text></setting>
                        <setting name="click">UserSuperUsersScreen.addUser()</setting>
                    </widget>
                </div>
            </content>
        </setting>
        <setting name="content">
            <widget type="GUI/Table" id="superUsersList">
                <setting name="columns">
                    <user>
                        <name><text>User</text></name>
                        <align>left</align>
                        <width>250px</width>
                    </user>
                    <email>
                        <name><text>Email</text></name>
                        <align>left</align>
                        <width>250px</width>
                    </email>
                    <lastLogin>
                        <name><text>Last Login</text></name>
                        <align>left</align>
                        <width>250px</width>
                    </lastLogin>
                </setting>
                <setting name="rows">
                    <dataProvider action="User::getSuperUsersList" />
                </setting>
                <setting name="rowGenerator">
                    <system>User</system>
                    <action>getSuperUserTableColumns</action>
                </setting>
                <setting name="alternate">cols</setting>
                <setting name="allowDelete">true</setting>
                <setting name="noItemsMsg"><text>There are no super users in the system</text></setting>
            </widget>
        </setting>
    </widget>
</div>
<script>
    /* Initialise the screen */
    UserSuperUsersScreen.init({screenData});
</script>

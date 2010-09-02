<div id="SuperUsersScreen">
    <widget type="GUI/Box" id="superUsers-box">
        <setting name="title"><text>Super Users</text></setting>
        <setting name="headerContent">
            <content>
                <div class="addNewButtonContainer">
                    <widget type="GUI/Button" id="addNewSuperUser">
                        <setting name="value"><text>Add Existing User</text></setting>
                        <setting name="click">SuperUsersScreen.addUser()</setting>
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
                    </user>
                    <email>
                        <name><text>Email</text></name>
                        <align>left</align>
                    </email>
                    <lastLogin>
                        <name><text>Last Login</text></name>
                        <align>left</align>
                    </lastLogin>
                </setting>
                <setting name="rows">
                    <dataProvider action="User::getSuperUsersList" />
                </setting>
                <setting name="rowGenerator">
                    <system>User</system>
                    <action>getSuperUserTableColumns</action>
                </setting>
                <setting name="alternate">rows</setting>
                <setting name="allowDelete">true</setting>
            </widget>
        </setting>
    </widget>
</div>
<var name="className"><content>UserManagerScreen</content></var>
<var name="groupInfo"><dataProvider action="User::getUserGroupInfoForTemplate" /></var>

<div id="{className}-editFolder">
    <div class="{className}-editFolder-top">
        <h2>{groupInfo:name}</h2>
    </div>
    <div id="{className}-createUser-wrapper">
        <!-- Group Info section -->
        <div id="{className}-infoSection">
            <div class="{className}-childUserDetails">
                <div class="{className}-assetIcon"></div> : {groupInfo:childUserCount}
                <widget type="GUI/Button" id="UserManagerScreen-addNewUser">
                    <setting name="value"><text>+ New</text></setting>
                    <setting name="click">UserUserManagerScreen.createNewUser();</setting>
                </widget>
                <widget type="GUI/Button" id="UserManagerScreen-addExistingUser">
                    <setting name="value"><text>+ Existing</text></setting>
                    <setting name="click">UserUserManagerScreen.addExistingUser();</setting>
                </widget>
            </div>
            <div class="{className}-childGroupDetails">
                <div class="{className}-assetIcon"></div> : {groupInfo:childGroupCount}
                <widget type="GUI/Button" id="UserManagerScreen-addNewGroup">
                    <setting name="value"><text>+ New</text></setting>
                    <setting name="click">UserUserManagerScreen.createNewUserGroup();</setting>
                </widget>
                <widget type="GUI/Button" id="UserManagerScreen-addExistingGroup">
                    <setting name="value"><text>+ Existing</text></setting>
                    <setting name="click">UserUserManagerScreen.addExistingUserGroup();</setting>
                </widget>
            </div>
        </div>

        <div id="{className}-createSection">
            <!-- Create User Section -->
            <div id="UserManagerScreen-createUser" style="display:none;">
                <div class="{className}-createUser-top">
                    <div class="{className}-assetIcon"></div><text>Create New User</text>
                    <widget type="GUI/Button" id="UserManagerScreen-cancelCreateUser">
                        <setting name="value"><text>Cancel</text></setting>
                        <setting name="colour">Light</setting>
                        <setting name="click">UserUserManagerScreen.cancelCreate();</setting>
                    </widget>
                </div>
                <div class="{className}-col-left">
                    <label for="UserManagerScreen-newUser-firstName-input"><text>First Name</text></label>
                    <widget type="GUI/TextBox" id="UserManagerScreen-newUser-firstName">
                        <setting name="size">17</setting>
                    </widget>
                </div>
                <div class="{className}-col-right">
                    <label for="UserManagerScreen-newUser-lastName-input"><text>Last Name</text></label>
                    <widget type="GUI/TextBox" id="UserManagerScreen-newUser-lastName">
                        <setting name="size">17</setting>
                    </widget>
                </div>
                <div class="{className}-fieldGap"></div>
                <label for="UserManagerScreen-newUser-email-input"><text>Email</text></label>
                <widget type="GUI/TextBox" id="UserManagerScreen-newUser-email">
                    <setting name="size">38</setting>
                </widget>

                <div class="{className}-fieldGap"></div>
                <label for="UserManagerScreen-newUser-username-input"><text>Username</text></label>
                <widget type="GUI/TextBox" id="UserManagerScreen-newUser-username" />

                <div class="{className}-fieldGap"></div>
                <label for="UserManagerScreen-newUser-password-input"><text>Password</text></label>
                <widget type="GUI/Password" id="UserManagerScreen-newUser-password" />

                <div class="{className}-editFolder-addToOtherGroup">
                </div>

                <widget type="GUI/Button" id="UserManagerScreen-newUser-createButton">
                    <setting name="value"><text>Create User</text></setting>
                    <setting name="click">UserUserManagerScreen.createUser();</setting>
                </widget>
            </div><!-- end Create User Section -->

            <!-- Create User Group Section -->
            <div id="UserManagerScreen-createGroup" style="display:none;">
                <div class="{className}-createUser-top">
                    <div class="{className}-assetIcon"></div><text>Create New User Group</text>
                    <widget type="GUI/Button" id="UserManagerScreen-cancelCreateUserGroup">
                        <setting name="value"><text>Cancel</text></setting>
                        <setting name="colour">Light</setting>
                        <setting name="click">UserUserManagerScreen.cancelCreate();</setting>
                    </widget>
                </div>
                <div class="{className}-editFolder-createGroup-col-left">
                    <label for="UserManagerScreen-newGroup-groupName-input"><text>Group Name</text></label>
                    <widget type="GUI/TextBox" id="UserManagerScreen-newGroup-groupName">
                        <setting name="size">20</setting>
                    </widget>
                </div>
                <div class="{className}-editFolder-addToOtherGroup">
                    <widget type="GUI/Button" id="UserManagerScreen-createGroup">
                        <setting name="value"><text>Create User Group</text></setting>
                        <setting name="click">UserUserManagerScreen.createUserGroup();</setting>
                    </widget>
                </div>
            </div><!-- end Create User Group Section -->
        </div>
    </div>
</div>

<var name="className"><content>UserManagerScreen</content></var>
<var name="groupInfo"><dataProvider action="User::getUserGroupInfoForTemplate" /></var>

<div id="{className}-editor" assetType="userGroup">
    <div class="{className}-editUserGroup-top">
        <h2>{groupInfo:name}</h2>
    </div>
    <div id="{className}-createUser-wrapper">
        <div class="{className}-editGroupName">
            <h5><text>Group Name</text></h5>
            <widget type="GUI/TextBox" id="UserManagerScreen-editUserGroup-groupName">
                <setting name="value">{groupInfo:name}</setting>
            </widget>
        </div>
        <div id="{className}-infoSection">
            <widget type="GUI/Box" id="{className}-infoSection-box">
                <setting name="title"><text>Group Members</text></setting>
                <setting name="content">
                    <content>
                        <div id="{className}-childUserDetailsWrapper">
                            <div class="{className}-childUserDetails">
                                <div class="{className}-assetIcon childUserCount"></div><span><text>Users</text>:</span> <span class="{className}-count {className}-count-user">{groupInfo:childUserCount}</span>
                                <div class="{className}-addMoreText {className}-addMoreText-user" onclick="dfx.toggleClass(dfx.getId('{className}-childUserDetailsWrapper'), 'active')"><span><text>Add More</text></span><div class="{className}-addMoreText-toggle"></div></div>
                            </div>
                            <div class="{className}-childButtons" id="{className}-UserButtons">
                                <widget type="GUI/Button" id="UserManagerScreen-addNewUser">
                                    <setting name="value"><text>Create New User</text></setting>
                                    <setting name="colour">Light</setting>
                                    <setting name="click">UserUserManagerScreen.showCreatePanel('createUser')</setting>
                                </widget>
                                &nbsp;
                                <widget type="GUI/Button" id="UserManagerScreen-addExistingUser">
                                    <setting name="value"><text>Link Existing</text></setting>
                                    <setting name="colour">Light</setting>
                                    <setting name="click">UserUserManagerScreen.addExistingAsset();</setting>
                                </widget>
                            </div>
                        </div>

                        <div id="{className}-childGroupDetailsWrapper">
                            <div class="{className}-childGroupDetails">
                                <div class="{className}-assetIcon childGroupCount"></div><span><text>Groups</text>:</span> <span class="{className}-count {className}-count-group">{groupInfo:childGroupCount}</span>
                                <div class="{className}-addMoreText {className}-addMoreText-group" onclick="dfx.toggleClass(dfx.getId('{className}-childGroupDetailsWrapper'), 'active')"><span><text>Add More</text></span><div class="{className}-addMoreText-toggle"></div></div>
                            </div>
                            <div class="{className}-childButtons" id="{className}-GroupButtons">
                                <widget type="GUI/Button" id="UserManagerScreen-addNewGroup">
                                    <setting name="value"><text>Create New Group</text></setting>
                                    <setting name="colour">Light</setting>
                                    <setting name="click">UserUserManagerScreen.showCreatePanel('createGroup')</setting>
                                </widget>
                                &nbsp;
                                <widget type="GUI/Button" id="UserManagerScreen-addExistingGroup">
                                    <setting name="value"><text>Link Existing</text></setting>
                                    <setting name="colour">Light</setting>
                                    <setting name="click">UserUserManagerScreen.addExistingAsset();</setting>
                                </widget>
                            </div>
                        </div>
                    </content>
                </setting>
            </widget>

            <div class="{className}-parentGroups">
                <h5><text>Belongs to Groups :</text></h5>
                <widget type="GUI/List" id="UserManagerScreen-parentsList">
                    <setting name="items">
                        <dataProvider action="User::getUserGroupsData" />
                    </setting>
                    <setting name="allowDelete">true</setting>
                    <setting name="itemsGenerator">
                        <system>User</system>
                        <action>getUserGroupData</action>
                    </setting>
                </widget>
            </div>
            <div class="{className}-addToOtherGroup">
                <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addToGroup">
                    <setting name="value"><text>Add to another Group</text></setting>
                    <setting name="click">UserUserManagerScreen.addToGroups();</setting>
                </widget>
            </div>
        </div>

        <div id="{className}-createSection" class="UserManagerScreen-createPanel">
            <!-- Create User Section -->
            <div id="UserManagerScreen-createUser" style="display:none;" class="UserManagerScreen-createPanel">
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
                        <setting name="enablesSaveButton">false</setting>
                    </widget>
                </div>
                <div class="{className}-col-right">
                    <label for="UserManagerScreen-newUser-lastName-input"><text>Last Name</text></label>
                    <widget type="GUI/TextBox" id="UserManagerScreen-newUser-lastName">
                        <setting name="size">17</setting>
                        <setting name="enablesSaveButton">false</setting>
                    </widget>
                </div>
                <div class="{className}-fieldGap"></div>
                <label for="UserManagerScreen-newUser-email-input"><text>Email</text></label>
                <widget type="GUI/TextBox" id="UserManagerScreen-newUser-email">
                    <setting name="size">38</setting>
                    <setting name="enablesSaveButton">false</setting>
                </widget>

                <div class="{className}-fieldGap"></div>
                <label for="UserManagerScreen-newUser-username-input"><text>Username</text></label>
                <widget type="GUI/TextBox" id="UserManagerScreen-newUser-username">
                    <setting name="enablesSaveButton">false</setting>
                </widget>

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
            <div id="UserManagerScreen-createGroup" style="display:none;" class="UserManagerScreen-createPanel">
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
                        <setting name="enablesSaveButton">false</setting>
                        <setting name="size">20</setting>
                    </widget>
                </div>
                <div class="{className}-editFolder-addToOtherGroup">
                    <widget type="GUI/Button" id="UserManagerScreen-createGroup-button">
                        <setting name="value"><text>Create User Group</text></setting>
                        <setting name="click">UserUserManagerScreen.createUserGroup();</setting>
                    </widget>
                </div>
            </div><!-- end Create User Group Section -->
        </div>
    </div>
</div>
<div id="userManager-deleteButton" onclick="UserUserManagerScreen.toggleMarkForDeletion();"></div>

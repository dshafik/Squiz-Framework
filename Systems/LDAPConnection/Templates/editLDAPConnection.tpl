<var name="ldapClassName"><content>LDAPUserManagerScreen</content></var>
<var name="className"><content>UserManagerScreen</content></var>
<var name="ldapInfo"><dataProvider action="LDAPConnection::getConnectionInfo" /></var>

<div id="{className}-editor" assetType="LDAPConnection" class="{ldapClassName}">
    <div class="{className}-editFolder-top">
        <h2>{ldapInfo:name}</h2>
    </div>
    <div id="{className}-createUser-wrapper">
        <h2><text>Details</text></h2>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-conName-input" class="GUI-requiredField"><text>Enter a name for the connection</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-name">
            <setting name="size">38</setting>
            <setting name="value">{ldapInfo:name}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <div class="{className}-col-left">
            <label for="{ldapClassName}-new-host-input" class="GUI-requiredField"><text>Host Name</text></label>
            <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-host">
                <setting name="size">20</setting>
                <setting name="value">{ldapInfo:details:host}</setting>
            </widget>
        </div>
        <div class="{className}-col-right">
            <label for="{ldapClassName}-new-port-input" class="GUI-requiredField"><text>Port</text></label>
            <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-port">
                <setting name="size">14</setting>
                <setting name="value">{ldapInfo:details:port}</setting>
            </widget>
        </div>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-baseDN-input" class="GUI-requiredField"><text>Base DN</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-baseDN">
            <setting name="size">38</setting>
            <setting name="value">{ldapInfo:details:baseDN}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-bindDN-input"><text>Bind DN</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-bindDN">
            <setting name="size">38</setting>
            <setting name="value">{ldapInfo:details:bindDN}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-password-input"><text>Password</text></label>
        <widget type="GUI/Password" id="LDAPUserManagerScreen-new-password" />

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-authDN-input"><text>Auth DN</text></label>
        <p><text>This field is used when authentication an LDAP user.
        If this is set, this DN will be used to search hfor the user being
        authentication instead of the base DN.</text>
        </p>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-authDN">
            <setting name="size">38</setting>
            <setting name="value">{ldapInfo:details:authDN}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-authFilter-input"><text>Auth Filter</text></label>
        <p><text>This field is used when authentication an LDAP user.
        If set, this filter will be applied when searching for the user being authenticated.
        If this field is left blank, no filter will be used for authentication.</text>
        </p>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-authFilter">
            <setting name="size">38</setting>
            <setting name="value">{ldapInfo:details:authFilter}</setting>
        </widget>

        <hr />
        <h2><text>User Attributes</text></h2>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-uid-input" class="GUI-requiredField"><text>Username</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-uid">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:userAttributes:uid}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-cn-input" class="GUI-requiredField"><text>Name</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-cn">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:userAttributes:cn}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-givenname-input" class="GUI-requiredField"><text>First Name</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-givenname">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:userAttributes:givenname}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-sn-input" class="GUI-requiredField"><text>Last Name</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-sn">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:userAttributes:sn}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-mail-input" class="GUI-requiredField"><text>Email</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-mail">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:userAttributes:mail}</setting>
        </widget>

        <hr />
        <h2><text>Group Attributes</text></h2>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-ou-input" class="GUI-requiredField"><text>Group Name</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-ou">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:groupAttributes:ou}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-groupMembership-input"><text>Group Membership</text></label>
        <p><text>If users store a list of their groups in an attribute, please enter it here.</text></p>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-groupMembership">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:groupAttributes:groupMembership}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-groupMembers-input"><text>Group Members</text></label>
        <p><text>If groups store a list of their members in an attribute, please enter it here.</text></p>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-groupMembers">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:groupAttributes:groupMembers}</setting>
        </widget>

        <hr />
        <h2><text>Unique ID Setting</text></h2>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-uniqueIdUsers-input"><text>Unique ID attribute for users</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-uniqueIdUsers">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:uniqueIdSettings:uniqueIdUsers}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <label for="{ldapClassName}-new-uniqueIdGroups-input"><text>Unique ID attribute for groups</text></label>
        <widget type="GUI/TextBox" id="LDAPUserManagerScreen-new-uniqueIdGroups">
            <setting name="size">20</setting>
            <setting name="value">{ldapInfo:uniqueIdSettings:uniqueIdGroups}</setting>
        </widget>
    </div>
</div>

<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<div id="BackupScreen" class="GUI-screen-middle">
    <h2><text>Available Backups</text></h2>
    <p><text>Restoring a backup is a two step process. When you click the restore button, a message will appear confirming your request.</text></p>
    <p><text>If you restore a backup, all of your content and settings will be restored to the way they were at this particular point in time.</text></p>
    <widget type="GUI/Box" id="backups-box">
        <setting name="title"><text>Available Backups</text></setting>
        <setting name="content">
            <widget type="GUI/Table" id="backupList">
                <setting name="columns">
                    <date>
                        <name><text>Date and Time</text></name>
                        <align>left</align>
                        <width>225px</width>
                    </date>
                    <size>
                        <name><text>Size</text></name>
                        <align>center</align>
                        <width>80px</width>
                    </size>
                    <download>
                        <name></name>
                        <align>left</align>
                        <width>70px</width>
                    </download>
                    <restore>
                        <name></name>
                        <align>right</align>
                        <width>70px</width>
                    </restore>
                </setting>
                <setting name="rows">
                    <dataProvider action="Backup::getBackupList" />
                </setting>
                <setting name="noItemsMsg"><text>No backups have been created</text></setting>
                <setting name="alternate">rows</setting>
            </widget>
        </setting>
    </widget>

    <widget type="GUI/Intervention" id="backupScreen-confirmRestore">
        <setting name="onclose">BackupBackupScreen.restore</setting>
        <setting name="elementContents">
            <content>
                <p><text>Are you sure you want to restore your system?</text></p>
                <widget type="GUI/Button" id="backupScreen-confirmRestore-buttonYes">
                    <setting name="value"><text>Restore</text></setting>
                    <setting name="colour">Dark</setting>
                    <setting name="click">GUI.getWidget('backupScreen-confirmRestore').hide(true);</setting>
                </widget>
                <widget type="GUI/Button" id="backupScreen-confirmRestore-buttonNo">
                    <setting name="value"><text>Cancel</text></setting>
                    <setting name="click">GUI.getWidget('backupScreen-confirmRestore').hide(false);</setting>
                    <setting name="colour">Dark</setting>
                </widget>
            </content>
        </setting>
    </widget>
</div>
